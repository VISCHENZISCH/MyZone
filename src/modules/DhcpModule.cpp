#include "../../include/modules/DhcpModule.hpp"

namespace myzone {

DhcpModule::DhcpModule(const Database& database)
    : database_(database) {}

std::string DhcpModule::name() const {
    return "DHCP";
}

bool DhcpModule::canEnrich(const DeviceProfile& profile) const {
    return !profile.dhcpFingerprint.empty();
}

void DhcpModule::enrich(DeviceProfile& profile) const {
    DhcpFingerprintInfo info;
    if (!database_.lookupDhcpFingerprint(profile.dhcpFingerprint, info)) {
        return;
    }

    profile.deviceModel = info.deviceName;

    if (profile.osName.empty()) {
        profile.osName = info.deviceName;
    }

    profile.addResult(name(), info.deviceName);
}

} // namespace myzone
