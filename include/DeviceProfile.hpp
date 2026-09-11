#ifndef MYZONE_DEVICE_PROFILE_HPP
#define MYZONE_DEVICE_PROFILE_HPP

#include <string>
#include <vector>
#include "MacAddress.hpp"
#include "DeviceCategory.hpp"

namespace myzone {

struct ModuleResult {
    std::string moduleName;
    std::string detail;
};

struct DeviceProfile {
    MacAddress mac;
    std::string dhcpFingerprint;
    std::string hostname;
    std::string tcpSignature;
    std::string dhcpOptions;

    std::string manufacturer;
    DeviceCategory category = DeviceCategory::Unknown;
    std::string osName;
    std::string deviceModel;
    std::vector<ModuleResult> results;

    void addResult(const std::string& moduleName, const std::string& detail);
    bool hasResults() const { return !results.empty(); }
};

} // namespace myzone

#endif // MYZONE_DEVICE_PROFILE_HPP
