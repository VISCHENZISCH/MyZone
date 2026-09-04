#ifndef MYZONE_DEVICE_CATEGORY_HPP
#define MYZONE_DEVICE_CATEGORY_HPP

#include <string>

namespace myzone {

// L'ensemble des catégories détectables sur le réseau
enum class DeviceCategory {
    // Équipements classiques
    Computer,       // PC de bureau, ordinateur portable (Windows, Mac, Linux)
    Mobile,         // Smartphones (Android, iPhone)
    Tablet,         // Tablettes (iPad, Galaxy Tab...)
    Server,         // Serveurs physiques ou NAS (Stockage réseau)
    VirtualMachine, // Machines virtuelles (VMware, VirtualBox, Hyper-V)

    // Équipements réseau (Infrastructure)
    Modem,          // Box internet, passerelles (Gateways)
    Router,         // Routeurs Wi-Fi et filaires
    Switch,         // Commutateurs réseau
    AccessPoint,    // Bornes Wi-Fi, répéteurs

    // Périphériques et Multimédia
    Printer,        // Imprimantes réseau, scanners
    SmartTV,        // Télévisions connectées, Box Android/Apple TV, Chromecast
    GamingConsole,  // Consoles de jeux (PlayStation, Xbox, Nintendo)
    Audio,          // Enceintes connectées (Sonos, Bose, HomePod)

    // Objets Connectés (IoT & Smart Home)
    Camera,         // Caméras IP, systèmes de sécurité (Arlo, Ring...)
    SmartHome,      // Domotique : ampoules, thermostats, prises (Philips Hue, Nest...)
    Wearable,       // Montres connectées, bracelets (Apple Watch, Garmin...)
    IoT,            // Autres objets connectés (catégorie générique)

    // Par défaut
    Unknown         // Pas de correspondance trouvée
};

// Fonction qui convertit un mot-clé (ex: "Printer", "Smartphone") vers une catégorie
DeviceCategory classifyFromDeviceType(const std::string& deviceType);

// Fonction qui retourne le nom lisible de la catégorie (pour l'afficher à l'écran)
std::string toString(DeviceCategory category);

}


#endif // MYZONE_DEVICE_CATEGORY_HPP