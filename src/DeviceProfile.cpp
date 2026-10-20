#include "../include/DeviceProfile.hpp"
#include <sstream>
#include <cstdio>

namespace myzone {

static std::string jsonEscape(const std::string& str) {
    std::string result;
    for (char c : str) {
        if (c == '"') {
            result += "\\\"";
        } else if (c == '\\') {
            result += "\\\\";
        } else if (c >= 0 && c < 0x20) {
            char buf[7];
            snprintf(buf, sizeof(buf), "\\u%04x", c);
            result += buf;
        } else {
            result += c;
        }
    }
    return result;
}

static std::string csvEscape(const std::string& str) {
    if (str.find(',') == std::string::npos && str.find('"') == std::string::npos && str.find('\n') == std::string::npos) {
        return str;
    }
    std::string result = "\"";
    for (char c : str) {
        if (c == '"') result += "\"\"";
        else result += c;
    }
    result += "\"";
    return result;
}

void DeviceProfile::addResult(const std::string& moduleName,
                              const std::string& detail) {
    results.push_back(ModuleResult{moduleName, detail});
}

int DeviceProfile::calculateConfidence() const {
    if (results.empty()) {
        return 0;
    }

    int score = 0;
    bool hasOui = false;
    bool hasDhcp = false;
    bool hasHostname = false;
    bool hasP0f = false;
    bool hasFingerBank = false;

    for (const auto& res : results) {
        if (res.moduleName == "OUI") hasOui = true;
        else if (res.moduleName == "DHCP") hasDhcp = true;
        else if (res.moduleName == "Hostname") hasHostname = true;
        else if (res.moduleName == "p0f TCP") hasP0f = true;
        else if (res.moduleName == "FingerBank") hasFingerBank = true;
    }

    if (hasOui) {
        if (mac.isValid() && mac.isLocallyAdministered()) {
            score += 15; // MAC privée reconnue heuristiquement
        } else if (!manufacturer.empty()) {
            score += 35; // Fabricant IEEE OUI officiel
        }
    }

    if (hasDhcp) score += 35;
    if (hasFingerBank) score += 25;
    if (hasHostname) score += 25;
    if (hasP0f) score += 25;

    // Bonus de corrélation croisée multi-sources
    if (!manufacturer.empty() && !osName.empty()) {
        score += 10;
    }

    if (score > 100) score = 100;
    return score;
}

std::string DeviceProfile::confidenceLevel() const {
    const int conf = calculateConfidence();
    if (conf >= 80) return "Élevé";
    if (conf >= 50) return "Moyen";
    if (conf > 0) return "Faible";
    return "Inconnu";
}

std::string DeviceProfile::toJson() const {
    std::stringstream ss;
    ss << "{";
    bool first = true;
    
    auto addField = [&ss, &first](const std::string& key, const std::string& value) {
        if (!value.empty()) {
            if (!first) ss << ",";
            ss << "\"" << key << "\":\"" << jsonEscape(value) << "\"";
            first = false;
        }
    };

    if (mac.isValid()) {
        addField("mac", mac.toString());
        if (!first) ss << ",";
        ss << "\"isRandomMac\":" << (mac.isLocallyAdministered() ? "true" : "false");
        first = false;
    }
    addField("manufacturer", manufacturer);
    if (category != DeviceCategory::Unknown) {
        addField("category", toString(category));
    }
    addField("osName", osName);
    addField("deviceModel", deviceModel);
    addField("hostname", hostname);
    addField("dhcpFingerprint", dhcpFingerprint);
    addField("tcpSignature", tcpSignature);
    addField("dhcpOptions", dhcpOptions);

    const int confidence = calculateConfidence();
    if (confidence > 0) {
        if (!first) ss << ",";
        ss << "\"confidence\":" << confidence << ",\"confidenceLevel\":\"" << confidenceLevel() << "\"";
        first = false;
    }
    
    if (!results.empty()) {
        if (!first) ss << ",";
        ss << "\"modules\":[";
        for (size_t i = 0; i < results.size(); ++i) {
            if (i > 0) ss << ",";
            ss << "{\"name\":\"" << jsonEscape(results[i].moduleName) 
               << "\",\"detail\":\"" << jsonEscape(results[i].detail) << "\"}";
        }
        ss << "]";
    }
    ss << "}";
    return ss.str();
}

std::string DeviceProfile::csvHeader() {
    return "mac,manufacturer,category,os_name,model,confidence,confidence_level,hostname,random_mac";
}

std::string DeviceProfile::toCsv() const {
    std::stringstream ss;
    ss << csvEscape(mac.isValid() ? mac.toString() : "") << ","
       << csvEscape(manufacturer) << ","
       << csvEscape(category != DeviceCategory::Unknown ? toString(category) : "") << ","
       << csvEscape(osName) << ","
       << csvEscape(deviceModel) << ","
       << calculateConfidence() << ","
       << csvEscape(confidenceLevel()) << ","
       << csvEscape(hostname) << ","
       << (mac.isValid() && mac.isLocallyAdministered() ? "true" : "false");
    return ss.str();
}

} // namespace myzone
