#ifndef MYZONE_IDENTIFICATION_ENGINE_HPP
#define MYZONE_IDENTIFICATION_ENGINE_HPP

#include <memory>
#include <vector>
#include "DeviceProfile.hpp"
#include "IdentificationModule.hpp"

namespace myzone {

/// @brief Moteur principal orchestrant l'identification d'appareils
class IdentificationEngine {
public:
    /// @brief Ajoute un module d'identification au moteur
    /// @param module Pointeur unique vers le module à ajouter
    void addModule(std::unique_ptr<IdentificationModule> module);
    /// @brief Identifie et enrichit un profil d'appareil à travers tous les modules
    /// @param input Le profil de base de l'appareil
    /// @return Un profil enrichi avec les résultats des modules
    DeviceProfile identify(const DeviceProfile& input) const;
    /// @brief Renvoie le nombre de modules enregistrés dans le moteur
    /// @return Le nombre de modules
    std::size_t moduleCount() const { return modules_.size(); }

private:
    std::vector<std::unique_ptr<IdentificationModule>> modules_;
};

} // namespace myzone

#endif // MYZONE_IDENTIFICATION_ENGINE_HPP
