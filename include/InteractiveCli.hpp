#ifndef MYZONE_INTERACTIVE_CLI_HPP
#define MYZONE_INTERACTIVE_CLI_HPP

#include "Database.hpp"
#include "DeviceProfile.hpp"
#include "IdentificationEngine.hpp"

namespace myzone {

/// @brief Gestionnaire de l'interface utilisateur interactive (TUI)
class InteractiveCli {
public:
    /// @brief Lance la boucle de menu interactif principale
    static void run(const Database& database, const IdentificationEngine& engine);

    /// @brief Affiche un profil d'appareil de manière formatée
    static void printProfile(const DeviceProfile& profile);

    /// @brief Invite l'utilisateur à saisir des paramètres pour identifier un appareil
    static void identifyDevice(const IdentificationEngine& engine);

    /// @brief Recherche une empreinte DHCP dans la base
    static void searchDhcp(const Database& database);

    /// @brief Affiche le catalogue des sources locales indexées
    static void showSources(const Database& database);

    /// @brief Exécute une démonstration automatisée sur des profils d'exemple
    static void runDemo(const IdentificationEngine& engine);

    /// @brief Affiche l'aide détaillée et les limites d'audit
    static void showHelp();
};

} // namespace myzone

#endif // MYZONE_INTERACTIVE_CLI_HPP
