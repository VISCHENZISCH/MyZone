#include "../../include/modules/OuiModule.hpp"

namespace myzone {

OuiModule::OuiModule(const Database& database)
    : database_(database) {}

std::string OuiModule::name() const {
    return "OUI";
}

bool OuiModule::canEnrich(const DeviceProfile& profile) const {
    return profile.mac.isValid();
}

void OuiModule::enrich(DeviceProfile& profile) const {
    if (profile.mac.isBroadcast()) {
        profile.addResult(name(), "Diffusion Broadcast (FF:FF:FF:FF:FF:FF)");
        return;
    }

    if (profile.mac.isMulticast()) {
        profile.addResult(name(), "Diffusion Multicast de groupe (bit I/G actif)");
        return;
    }

    const bool isRandom = profile.mac.isLocallyAdministered();

    DeviceInfo info;
    if (database_.lookup(profile.mac, info)) {
        profile.manufacturer = info.companyName;
        if (info.category != DeviceCategory::Unknown) {
            profile.category = info.category;
        }

        std::string detail = info.companyName + " (" + toString(info.category) + ")";
        if (isRandom) {
            detail += " [Avertissement: MAC privée/aléatoire détectée]";
        }
        profile.addResult(name(), detail);
    } else if (isRandom) {
        profile.addResult(name(), "MAC privée / aléatoire détectée (bit LAA actif - masquage d'identité iOS/Android/Windows)");
    }
}

} // namespace myzone
