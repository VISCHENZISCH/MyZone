#ifndef MYZONE_DEVICE_CATEGORY_HPP
#define MYZONE_DEVICE_CATEGORY_HPP

#include <string>

namespace myzone {

/// @brief Énumération des différentes catégories d'appareils
enum class DeviceCategory {
    /// @brief Ordinateur de bureau ou portable
    Computer,
    /// @brief Téléphone mobile
    Mobile,
    /// @brief Tablette
    Tablet,
    /// @brief Serveur
    Server,
    /// @brief Machine virtuelle
    VirtualMachine,

    /// @brief Modem
    Modem,
    /// @brief Routeur
    Router,
    /// @brief Commutateur (Switch) réseau
    Switch,
    /// @brief Point d'accès sans fil
    AccessPoint,

    /// @brief Imprimante
    Printer,
    /// @brief Télévision connectée (Smart TV)
    SmartTV,
    /// @brief Console de jeux
    GamingConsole,
    /// @brief Équipement audio
    Audio,

    /// @brief Caméra
    Camera,
    /// @brief Équipement domotique (Smart Home)
    SmartHome,
    /// @brief Appareil portable (Montre connectée, etc.)
    Wearable,
    /// @brief Objet connecté (Internet of Things)
    IoT,

    /// @brief Catégorie inconnue
    Unknown
};

/// @brief Classifie un appareil à partir de sa chaîne de type textuelle
/// @param deviceType Le type d'appareil sous forme de chaîne
/// @return La catégorie d'appareil correspondante
DeviceCategory classifyFromDeviceType(const std::string& deviceType);
/// @brief Convertit une DeviceCategory en chaîne de caractères
/// @param category La catégorie à convertir
/// @return Le nom de la catégorie sous forme de chaîne
std::string toString(DeviceCategory category);

}

#endif // MYZONE_DEVICE_CATEGORY_HPP