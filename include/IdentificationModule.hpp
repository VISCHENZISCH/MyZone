#ifndef MYZONE_IDENTIFICATION_MODULE_HPP
#define MYZONE_IDENTIFICATION_MODULE_HPP

#include <string>
#include "DeviceProfile.hpp"

namespace myzone {

/// @brief Interface de base pour un module d'identification d'appareils
class IdentificationModule {
public:
    /// @brief Destructeur virtuel
    virtual ~IdentificationModule() = default;
    /// @brief Renvoie le nom du module d'identification
    /// @return Nom du module
    virtual std::string name() const = 0;
    /// @brief Vérifie si ce module a la capacité d'enrichir ce profil spécifique
    /// @param profile Le profil de l'appareil
    /// @return true si le module peut contribuer, false sinon
    virtual bool canEnrich(const DeviceProfile& profile) const = 0;
    /// @brief Enrichit le profil de l'appareil avec de nouvelles informations
    /// @param profile Le profil à enrichir (modifié sur place)
    virtual void enrich(DeviceProfile& profile) const = 0;
};

} // namespace myzone

#endif // MYZONE_IDENTIFICATION_MODULE_HPP
