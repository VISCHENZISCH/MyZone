#ifndef MYZONE_DEVICE_CATEGORY_HPP
#define MYZONE_DEVICE_CATEGORY_HPP

#include <string>

namespace myzone {

// Les 3 grandes catégories ciblées par MyZone, + Inconnu par défaut.
enum class DeviceCategory {
    Modem,      // box internet / modem-routeur
    Wifi,       // équipement réseau : routeur, switch, access point
    IoT,        // objets connectés : caméra, smart home, wearable...
    Unknown     // pas de correspondance trouvée
};

// Convertit le champ "device_type" du CSV OUI (ex: "Router", "IoT", "Camera")
// vers une des catégories MyZone.
DeviceCategory classifyFromDeviceType(const std::string& deviceType);

// Retourne le nom lisible d'une catégorie (pour l'affichage CLI)
std::string toString(DeviceCategory category);

} // namespace myzone

#endif // MYZONE_DEVICE_CATEGORY_HPP
