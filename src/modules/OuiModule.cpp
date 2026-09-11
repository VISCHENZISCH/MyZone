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
    DeviceInfo info;
    if (!database_.lookup(profile.mac, info)) {
        return;
    }

    profile.manufacturer = info.companyName;

    if (info.category != DeviceCategory::Unknown) {
        profile.category = info.category;
    }

    profile.addResult(name(),
                      info.companyName + " (" + toString(info.category) + ")");
}

} // namespace myzone
