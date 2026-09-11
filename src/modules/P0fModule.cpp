#include "../../include/modules/P0fModule.hpp"

namespace myzone {

P0fModule::P0fModule(const Database& database)
    : database_(database) {}

std::string P0fModule::name() const {
    return "p0f TCP";
}

bool P0fModule::canEnrich(const DeviceProfile& profile) const {
    return !profile.tcpSignature.empty();
}

void P0fModule::enrich(DeviceProfile& profile) const {
    P0fSignatureInfo info;
    if (!database_.lookupP0fSignature(profile.tcpSignature, info)) {
        return;
    }

    if (profile.osName.empty()) {
        profile.osName = info.osName;
    }

    profile.addResult(name(), info.osName);
}

} // namespace myzone
