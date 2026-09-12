#ifndef MYZONE_DEVICE_PROFILE_HPP
#define MYZONE_DEVICE_PROFILE_HPP

#include <string>
#include <vector>
#include "MacAddress.hpp"
#include "DeviceCategory.hpp"

namespace myzone {

/// @brief Résultat produit par un module d'identification
struct ModuleResult {
    /// @brief Nom du module ayant produit le résultat
    std::string moduleName;
    /// @brief Détails ou conclusion fournie par le module
    std::string detail;
};

/// @brief Profil complet d'un appareil identifié sur le réseau
struct DeviceProfile {
    /// @brief Adresse MAC de l'appareil
    MacAddress mac;
    /// @brief Empreinte DHCP brute
    std::string dhcpFingerprint;
    /// @brief Nom d'hôte de l'appareil
    std::string hostname;
    /// @brief Signature TCP (souvent utilisée pour p0f)
    std::string tcpSignature;
    /// @brief Options DHCP
    std::string dhcpOptions;

    /// @brief Fabricant ou marque de l'appareil
    std::string manufacturer;
    /// @brief Catégorie de l'appareil
    DeviceCategory category = DeviceCategory::Unknown;
    /// @brief Nom du système d'exploitation
    std::string osName;
    /// @brief Modèle de l'appareil
    std::string deviceModel;
    /// @brief Liste des résultats des différents modules d'identification
    std::vector<ModuleResult> results;

    /// @brief Ajoute le résultat d'un module au profil
    /// @param moduleName Le nom du module
    /// @param detail Les détails trouvés
    void addResult(const std::string& moduleName, const std::string& detail);
    /// @brief Vérifie si des résultats de modules sont présents
    /// @return true s'il y a des résultats, false sinon
    bool hasResults() const { return !results.empty(); }
    /// @brief Sérialise le profil de l'appareil au format JSON
    /// @return Une chaîne de caractères contenant le JSON
    std::string toJson() const;
};

} // namespace myzone

#endif // MYZONE_DEVICE_PROFILE_HPP
