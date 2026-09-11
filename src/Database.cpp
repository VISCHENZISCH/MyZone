#include "../include/Database.hpp"

#include <algorithm>
#include <array>
#include <cctype>
#include <fstream>
#include <iostream>
#include <string_view>

namespace myzone {

namespace {

std::string trim(const std::string& value) {
    const auto first = std::find_if_not(value.begin(), value.end(), [](unsigned char c) {
        return std::isspace(c) != 0;
    });
    const auto last = std::find_if_not(value.rbegin(), value.rend(), [](unsigned char c) {
        return std::isspace(c) != 0;
    }).base();
    return first >= last ? "" : std::string(first, last);
}

std::string lower(std::string value) {
    std::transform(value.begin(), value.end(), value.begin(), [](unsigned char c) {
        return static_cast<char>(std::tolower(c));
    });
    return value;
}

std::string normalizeDhcpHash(const std::string& raw) {
    std::string hash = lower(trim(raw));
    if (hash.size() != 32) {
        return "";
    }
    for (const unsigned char character : hash) {
        if (!std::isxdigit(character)) {
            return "";
        }
    }
    return hash;
}

bool isSupportedPrefixLength(const std::size_t length) {
    return length == 6 || length == 7 || length == 9;
}

std::string normalizePrefix(const std::string& raw) {
    const std::string value = trim(raw);
    const std::size_t slash = value.find('/');
    const std::string_view address(value.data(), slash == std::string::npos ? value.size() : slash);

    std::string prefix;
    prefix.reserve(9);
    for (const unsigned char character : address) {
        if (std::isxdigit(character)) {
            prefix += static_cast<char>(std::toupper(character));
        } else if (character != ':' && character != '-') {
            return "";
        }
    }

    if (slash == std::string::npos) {
        return isSupportedPrefixLength(prefix.size()) ? prefix : "";
    }

    const std::string_view mask(value.data() + slash + 1, value.size() - slash - 1);
    if (mask.empty() || value.find('/', slash + 1) != std::string::npos) {
        return "";
    }

    unsigned int bits = 0;
    for (const unsigned char character : mask) {
        if (!std::isdigit(character)) {
            return "";
        }
        bits = bits * 10U + static_cast<unsigned int>(character - '0');
        if (bits > 48U) {
            return "";
        }
    }

    const std::size_t hexDigits = bits / 4U;
    if (bits % 4U != 0U || !isSupportedPrefixLength(hexDigits) || prefix.size() < hexDigits) {
        return "";
    }
    prefix.resize(hexDigits);
    return prefix;
}

struct LookupRow {
    std::string prefix;
    std::string company;
    std::string deviceType;
};

bool parseLookupRow(const std::string& line, LookupRow& row) {
    int column = 0;
    bool inQuotes = false;
    std::string field;

    const auto storeField = [&]() {
        if (column == 0) {
            row.prefix = trim(field);
        } else if (column == 1) {
            row.company = trim(field);
        } else if (column == 4) {
            row.deviceType = trim(field);
        }
        field.clear();
        ++column;
    };

    for (std::size_t index = 0; index < line.size(); ++index) {
        const char character = line[index];
        const bool neededColumn = column == 0 || column == 1 || column == 4;

        if (character == '"') {
            if (inQuotes && index + 1 < line.size() && line[index + 1] == '"') {
                if (neededColumn) {
                    field += character;
                }
                ++index;
            } else {
                inQuotes = !inQuotes;
            }
        } else if (character == ',' && !inQuotes) {
            storeField();
        } else if (neededColumn) {
            field += character;
        }
    }
    storeField();
    return column >= 5;
}

std::string tabField(const std::string& line, const std::size_t index) {
    std::size_t begin = 0;
    for (std::size_t current = 0; current < index; ++current) {
        begin = line.find('\t', begin);
        if (begin == std::string::npos) {
            return "";
        }
        ++begin;
    }
    const std::size_t end = line.find('\t', begin);
    return trim(line.substr(begin, end == std::string::npos ? std::string::npos : end - begin));
}

std::string joinPath(const std::string& directory, const std::string& filename) {
    if (directory.empty() || directory == ".") {
        return filename;
    }
    const char last = directory.back();
    return directory + (last == '/' || last == '\\' ? "" : "/") + filename;
}

bool fileExists(const std::string& filePath) {
    std::ifstream file(filePath);
    return file.good();
}

} // namespace

Database::Database(const std::string& dataDirectory) : dataDirectory_(dataDirectory) {
    data_.max_load_factor(0.70F);
    data_.reserve(150000);
    dhcpData_.max_load_factor(0.70F);
    dhcpData_.reserve(1024);
    p0fData_.max_load_factor(0.70F);
    p0fData_.reserve(1024);
    sources_.reserve(9);

    loadLookupCsv(joinPath(dataDirectory_, "lookup.csv"));
    loadManufFile(joinPath(dataDirectory_, "manuf"), "Wireshark manuf");
    loadManufFile(joinPath(dataDirectory_, "wireshark-manuf.txt"), "Wireshark manuf historique");
    loadNmapPrefixes(joinPath(dataDirectory_, "nmap-mac-prefixes.txt"));
    loadDhcpFingerprints(joinPath(dataDirectory_, "kyd-dhcp-db.txt"));
    loadP0fSignatures(joinPath(dataDirectory_, "p0f.fp"));
    loadFingerBankConf(joinPath(dataDirectory_, "dhcp_fingerprints.conf"));

    addPassiveSource("nmap-os-db.txt", "Nmap OS DB",
                     "Signatures pour le fingerprinting actif des systèmes");
    addPassiveSource("nmap-service-probes.txt", "Nmap service probes",
                     "Probes et signatures de services réseau");
}

void Database::addSource(const DataSourceInfo& source) {
    sources_.push_back(source);
}

void Database::addVendor(const std::string& prefix,
                         const std::string& companyName,
                         const DeviceCategory category,
                         const std::string& source) {
    if (prefix.empty() || companyName.empty()) {
        return;
    }

    const auto [entry, inserted] = data_.try_emplace(prefix);
    if (inserted) {
        entry->second = DeviceInfo{companyName, category, source};
    }
}

void Database::loadLookupCsv(const std::string& filePath) {
    std::ifstream file(filePath);
    DataSourceInfo source{"lookup.csv", "Référentiel OUI MyZone",
                          "Fabricant et catégorie d'appareil", 0, file.is_open(), true};
    if (!file.is_open()) {
        std::cerr << "Impossible d'ouvrir la base OUI : " << filePath << std::endl;
        addSource(source);
        return;
    }

    std::string line;
    LookupRow row;
    while (std::getline(file, line)) {
        if (line.empty() || !parseLookupRow(line, row) || row.prefix == "oui") {
            continue;
        }

        const std::string prefix = normalizePrefix(row.prefix);
        if (prefix.empty()) {
            continue;
        }
        addVendor(prefix, row.company, classifyFromDeviceType(row.deviceType), "Référentiel OUI MyZone");
        ++source.records;
    }
    addSource(source);
}

void Database::loadManufFile(const std::string& filePath, const std::string& sourceName) {
    std::ifstream file(filePath);
    const std::string filename = filePath.substr(filePath.find_last_of("/\\") + 1);
    DataSourceInfo source{filename, sourceName, "Fallback fabricant OUI", 0, file.is_open(), true};
    if (!file.is_open()) {
        addSource(source);
        return;
    }

    std::string line;
    while (std::getline(file, line)) {
        if (line.empty() || line[0] == '#') {
            continue;
        }

        const std::string prefix = normalizePrefix(tabField(line, 0));
        std::string vendor = tabField(line, 2);
        if (vendor.empty()) {
            vendor = tabField(line, 1);
        }
        const std::size_t comment = vendor.find('#');
        if (comment != std::string::npos) {
            const std::string fullName = trim(vendor.substr(comment + 1));
            vendor = fullName.empty() ? trim(vendor.substr(0, comment)) : fullName;
        }
        if (!vendor.empty() && vendor[0] == '#') {
            vendor = trim(vendor.substr(1));
        }

        if (prefix.empty() || vendor.empty()) {
            continue;
        }
        addVendor(prefix, vendor, DeviceCategory::Unknown, sourceName);
        ++source.records;
    }
    addSource(source);
}

void Database::loadNmapPrefixes(const std::string& filePath) {
    std::ifstream file(filePath);
    DataSourceInfo source{"nmap-mac-prefixes.txt", "Nmap MAC prefixes",
                          "Fallback fabricant OUI", 0, file.is_open(), true};
    if (!file.is_open()) {
        addSource(source);
        return;
    }

    std::string line;
    while (std::getline(file, line)) {
        if (line.empty() || line[0] == '#') {
            continue;
        }
        const std::size_t separator = line.find_first_of(" \t");
        if (separator == std::string::npos) {
            continue;
        }

        const std::string prefix = normalizePrefix(line.substr(0, separator));
        const std::string vendor = trim(line.substr(separator + 1));
        if (prefix.empty() || vendor.empty()) {
            continue;
        }
        addVendor(prefix, vendor, DeviceCategory::Unknown, "Nmap MAC prefixes");
        ++source.records;
    }
    addSource(source);
}

void Database::loadDhcpFingerprints(const std::string& filePath) {
    std::ifstream file(filePath);
    DataSourceInfo source{"kyd-dhcp-db.txt", "KYD / FingerBank DHCP",
                          "Identification d'appareil par empreinte DHCP", 0, file.is_open(), true};
    if (!file.is_open()) {
        addSource(source);
        return;
    }

    std::string line;
    while (std::getline(file, line)) {
        if (line.empty() || line[0] == '#') {
            continue;
        }

        DhcpFingerprintInfo info;
        info.fingerprint = normalizeDhcpHash(tabField(line, 0));
        info.requestedOptions = tabField(line, 1);
        info.deviceName = tabField(line, 2);
        const std::string confidence = tabField(line, 3);
        if (info.fingerprint.empty() || info.deviceName.empty() || confidence.empty()) {
            continue;
        }

        try {
            info.confidence = std::stoi(confidence);
        } catch (const std::exception&) {
            continue;
        }

        dhcpData_.try_emplace(info.fingerprint, std::move(info));
        ++source.records;
    }
    addSource(source);
}

void Database::loadP0fSignatures(const std::string& filePath) {
    std::ifstream file(filePath);
    DataSourceInfo source{"p0f.fp", "p0f",
                          "Signatures pour le fingerprinting passif TCP/IP", 0, file.is_open(), true};
    if (!file.is_open()) {
        addSource(source);
        return;
    }

    std::string line;
    bool inTcpRequest = false;
    std::string currentLabel;

    while (std::getline(file, line)) {
        std::string tLine = trim(line);
        if (tLine.empty() || tLine[0] == ';') continue;

        if (tLine.front() == '[' && tLine.back() == ']') {
            inTcpRequest = (tLine == "[tcp:request]");
            currentLabel.clear();
            continue;
        }

        if (inTcpRequest) {
            if (tLine.rfind("label", 0) == 0) {
                std::size_t eqPos = tLine.find('=');
                if (eqPos != std::string::npos) {
                    currentLabel = trim(tLine.substr(eqPos + 1));
                }
            } else if (tLine.rfind("sig", 0) == 0) {
                std::size_t eqPos = tLine.find('=');
                if (eqPos != std::string::npos && !currentLabel.empty()) {
                    std::string sig = trim(tLine.substr(eqPos + 1));
                    if (!sig.empty()) {
                        P0fSignatureInfo info;
                        info.signature = sig;
                        // Clean up label if it's s:class:name:version
                        if (currentLabel.rfind("s:", 0) == 0) {
                            std::string cleaned;
                            for (std::size_t i = 2; i < currentLabel.size(); ++i) {
                                if (currentLabel[i] == ':') cleaned += ' ';
                                else cleaned += currentLabel[i];
                            }
                            info.osName = cleaned;
                        } else {
                            info.osName = currentLabel;
                        }
                        p0fData_.try_emplace(sig, std::move(info));
                        ++source.records;
                    }
                }
            }
        }
    }
    addSource(source);
}

void Database::loadFingerBankConf(const std::string& filePath) {
    std::ifstream file(filePath);
    DataSourceInfo source{"dhcp_fingerprints.conf", "FingerBank DHCP",
                          "Règles de signature DHCP complémentaires", 0, file.is_open(), true};
    if (!file.is_open()) {
        addSource(source);
        return;
    }

    std::string line;
    bool inOsSection = false;
    bool readingFingerprints = false;
    std::string currentOs;

    while (std::getline(file, line)) {
        std::string tLine = trim(line);
        if (tLine.empty() || tLine[0] == '#') continue;

        if (tLine.front() == '[' && tLine.back() == ']') {
            inOsSection = (tLine.rfind("[os ", 0) == 0);
            readingFingerprints = false;
            currentOs.clear();
            continue;
        }

        if (inOsSection) {
            if (tLine.rfind("description=", 0) == 0) {
                currentOs = trim(tLine.substr(12));
            } else if (tLine.rfind("fingerprints=<<EOT", 0) == 0) {
                readingFingerprints = true;
            } else if (tLine == "EOT") {
                readingFingerprints = false;
            } else if (readingFingerprints && !currentOs.empty()) {
                // Cette ligne contient la liste des options (ex: "1,15,3,6,44")
                FingerBankInfo info;
                info.options = tLine;
                info.osName = currentOs;
                fingerBankData_.try_emplace(tLine, std::move(info));
                ++source.records;
            }
        }
    }
    addSource(source);
}

void Database::addPassiveSource(const std::string& fileName,
                                const std::string& label,
                                const std::string& purpose) {
    const bool available = fileExists(joinPath(dataDirectory_, fileName));
    addSource(DataSourceInfo{fileName, label, purpose, 0, available, false});
}

bool Database::lookup(const MacAddress& mac, DeviceInfo& outInfo) const {
    if (!mac.isValid()) {
        return false;
    }

    const std::string address = mac.compact();
    static constexpr std::array<std::size_t, 3> prefixLengths = {9, 7, 6};
    for (const std::size_t length : prefixLengths) {
        const auto it = data_.find(address.substr(0, length));
        if (it != data_.end()) {
            outInfo = it->second;
            return true;
        }
    }
    return false;
}

bool Database::lookupDhcpFingerprint(const std::string& fingerprint,
                                     DhcpFingerprintInfo& outInfo) const {
    const std::string normalized = normalizeDhcpHash(fingerprint);
    if (normalized.empty()) {
        return false;
    }
    const auto it = dhcpData_.find(normalized);
    if (it == dhcpData_.end()) {
        return false;
    }
    outInfo = it->second;
    return true;
}

bool Database::lookupP0fSignature(const std::string& signature,
                                  P0fSignatureInfo& outInfo) const {
    const auto it = p0fData_.find(trim(signature));
    if (it == p0fData_.end()) {
        return false;
    }
    outInfo = it->second;
    return true;
}

bool Database::lookupFingerBankOptions(const std::string& options,
                                       FingerBankInfo& outInfo) const {
    const auto it = fingerBankData_.find(trim(options));
    if (it == fingerBankData_.end()) {
        return false;
    }
    outInfo = it->second;
    return true;
}

} // namespace myzone
