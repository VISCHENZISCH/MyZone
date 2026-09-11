#include "../../include/modules/HostnameModule.hpp"

#include <regex>
#include <string>

namespace myzone {

std::string HostnameModule::name() const {
    return "Hostname";
}

bool HostnameModule::canEnrich(const DeviceProfile& profile) const {
    return !profile.hostname.empty();
}

void HostnameModule::enrich(DeviceProfile& profile) const {
    const std::string& host = profile.hostname;

    // Expressions régulières pour les patterns connus
    const std::regex winDesktop("^DESKTOP-[A-Z0-9]{7}$", std::regex_constants::icase);
    const std::regex winLaptop("^LAPTOP-[A-Z0-9]{7}$", std::regex_constants::icase);
    const std::regex android("^android-[a-f0-9]+$", std::regex_constants::icase);
    const std::regex galaxy("^Galaxy-.*", std::regex_constants::icase);
    const std::regex pixel("^Pixel-.*", std::regex_constants::icase);
    const std::regex iphone("(iPhone|iPhone-de-.*|.*s-iPhone|.*-iPhone)$", std::regex_constants::icase);
    const std::regex ipad("(iPad|iPad-de-.*|.*s-iPad|.*-iPad)$", std::regex_constants::icase);
    const std::regex macbook(".*MacBook.*", std::regex_constants::icase);

    std::string detail;

    if (std::regex_match(host, winDesktop)) {
        profile.osName = "Windows";
        profile.category = DeviceCategory::Computer;
        detail = "Format Windows Desktop détecté";
    } else if (std::regex_match(host, winLaptop)) {
        profile.osName = "Windows";
        profile.category = DeviceCategory::Computer;
        detail = "Format Windows Laptop détecté";
    } else if (std::regex_match(host, android)) {
        profile.osName = "Android";
        profile.category = DeviceCategory::Mobile;
        detail = "Format générique Android détecté";
    } else if (std::regex_match(host, galaxy)) {
        profile.manufacturer = "Samsung";
        profile.osName = "Android";
        profile.category = DeviceCategory::Mobile;
        detail = "Gamme Samsung Galaxy détectée";
    } else if (std::regex_match(host, pixel)) {
        profile.manufacturer = "Google";
        profile.osName = "Android";
        profile.category = DeviceCategory::Mobile;
        detail = "Gamme Google Pixel détectée";
    } else if (std::regex_match(host, iphone)) {
        profile.manufacturer = "Apple, Inc.";
        profile.osName = "Apple iOS";
        profile.category = DeviceCategory::Mobile;
        detail = "Pattern iPhone détecté";
    } else if (std::regex_match(host, ipad)) {
        profile.manufacturer = "Apple, Inc.";
        profile.osName = "Apple iOS";
        profile.category = DeviceCategory::Tablet;
        detail = "Pattern iPad détecté";
    } else if (std::regex_match(host, macbook)) {
        profile.manufacturer = "Apple, Inc.";
        profile.osName = "macOS";
        profile.category = DeviceCategory::Computer;
        detail = "Pattern MacBook détecté";
    } else {
        return; // Aucun match
    }

    profile.addResult(name(), detail);
}

} // namespace myzone
