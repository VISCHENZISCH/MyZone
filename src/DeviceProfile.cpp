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

void DeviceProfile::addResult(const std::string& moduleName,
                              const std::string& detail) {
    results.push_back(ModuleResult{moduleName, detail});
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

} // namespace myzone
