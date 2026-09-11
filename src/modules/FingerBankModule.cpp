#include "../../include/modules/FingerBankModule.hpp"

namespace myzone {

FingerBankModule::FingerBankModule(const Database& database)
    : database_(database) {}

std::string FingerBankModule::name() const {
    return "FingerBank";
}

bool FingerBankModule::canEnrich(const DeviceProfile& profile) const {
    return !profile.dhcpOptions.empty();
}

void FingerBankModule::enrich(DeviceProfile& profile) const {
    FingerBankInfo info;
    if (!database_.lookupFingerBankOptions(profile.dhcpOptions, info)) {
        return;
    }

    if (profile.osName.empty()) {
        profile.osName = info.osName;
    }

    profile.addResult(name(), info.osName);
}

} // namespace myzone
