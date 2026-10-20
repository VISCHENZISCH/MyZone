#ifndef MYZONE_DATA_LOCATOR_HPP
#define MYZONE_DATA_LOCATOR_HPP

#include <string>

namespace myzone {

/// @brief Utilitaire pour localiser automatiquement le répertoire des données 'data/'
class DataLocator {
public:
    /// @brief Tente de localiser le répertoire de données 'data' à partir du chemin de l'exécutable et du répertoire courant
    /// @param executablePath Chemin d'invocation de l'exécutable (ex: argv[0])
    /// @return Chemin valide vers le dossier data/ ou "data" par défaut
    static std::string locate(const char* executablePath = nullptr);
};

} // namespace myzone

#endif // MYZONE_DATA_LOCATOR_HPP
