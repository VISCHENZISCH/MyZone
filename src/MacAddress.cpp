#include "../include/MacAddress.hpp"

#include <algorithm>
#include <cctype>
#include <stdexcept>

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

int hexValue(unsigned char value) {
    if (value >= '0' && value <= '9') {
        return value - '0';
    }
    value = static_cast<unsigned char>(std::toupper(value));
    return value >= 'A' && value <= 'F' ? value - 'A' + 10 : -1;
}

} // namespace

std::string MacAddress::normalizeInput(const std::string& raw) {
    const std::string value = trim(raw);
    std::string normalized;
    normalized.reserve(12);

    if (value.size() == 12) {
        for (unsigned char c : value) {
            if (!std::isxdigit(c)) {
                return "";
            }
            normalized += static_cast<char>(std::toupper(c));
        }
        return normalized;
    }

    if (value.size() != 17 || (value[2] != ':' && value[2] != '-')) {
        return "";
    }
    const char separator = value[2];
    for (std::size_t index = 0; index < value.size(); ++index) {
        if (index == 2 || index == 5 || index == 8 || index == 11 || index == 14) {
            if (value[index] != separator) {
                return "";
            }
        } else {
            const unsigned char c = static_cast<unsigned char>(value[index]);
            if (!std::isxdigit(c)) {
                return "";
            }
            normalized += static_cast<char>(std::toupper(c));
        }
    }
    return normalized;
}

MacAddress::MacAddress(const std::string& raw) {
    const std::string clean = normalizeInput(raw);
    if (clean.size() != 12) {
        throw std::invalid_argument("Format d'adresse MAC invalide : " + raw);
    }

    for (std::size_t index = 0; index < bytes_.size(); ++index) {
        const int high = hexValue(static_cast<unsigned char>(clean[index * 2]));
        const int low = hexValue(static_cast<unsigned char>(clean[index * 2 + 1]));
        if (high < 0 || low < 0) {
            throw std::invalid_argument("Erreur de parsing de l'adresse MAC : " + raw);
        }
        bytes_[index] = static_cast<unsigned char>((high << 4) | low);
    }
    valid_ = true;
}

std::string MacAddress::compact() const {
    if (!valid_) {
        return "";
    }

    static constexpr char hex[] = "0123456789ABCDEF";
    std::string result;
    result.reserve(12);
    for (const unsigned char byte : bytes_) {
        result += hex[byte >> 4];
        result += hex[byte & 0x0F];
    }
    return result;
}

std::string MacAddress::toString() const {
    const std::string value = compact();
    if (value.empty()) {
        return "INVALID_MAC";
    }

    std::string result;
    result.reserve(17);
    for (std::size_t index = 0; index < value.size(); ++index) {
        if (index > 0 && index % 2 == 0) {
            result += ':';
        }
        result += value[index];
    }
    return result;
}

bool MacAddress::operator==(const MacAddress& other) const {
    if (valid_ != other.valid_) return false;
    if (!valid_) return true;
    return bytes_ == other.bytes_;
}

bool MacAddress::operator!=(const MacAddress& other) const {
    return !(*this == other);
}

bool MacAddress::operator<(const MacAddress& other) const {
    if (valid_ != other.valid_) {
        return valid_ < other.valid_; // invalid < valid
    }
    if (!valid_) {
        return false;
    }
    return bytes_ < other.bytes_;
}

std::optional<MacAddress> MacAddress::tryParse(const std::string& raw) noexcept {
    try {
        return MacAddress(raw);
    } catch (const std::invalid_argument&) {
        return std::nullopt;
    }
}

} // namespace myzone

namespace std {
std::size_t hash<myzone::MacAddress>::operator()(const myzone::MacAddress& mac) const noexcept {
    if (!mac.isValid()) {
        return 0;
    }
    std::size_t h = 14695981039346656037ULL;
    for (unsigned char b : mac.bytes()) {
        h ^= b;
        h *= 1099511628211ULL;
    }
    return h;
}
}
