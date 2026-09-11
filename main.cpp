#include <cstdlib>
#include <filesystem>
#include <iostream>
#include <stdexcept>
#include <string>
#include <vector>

#include "include/Database.hpp"
#include "include/DeviceCategory.hpp"
#include "include/MacAddress.hpp"

#include "include/UI.hpp"

namespace fs = std::filesystem;

namespace {

std::string locateDataDirectory(const char* executable) {
    std::vector<fs::path> roots;
    roots.push_back(fs::current_path());

    std::error_code error;
    const fs::path executablePath = fs::absolute(executable, error);
    if (!error) {
        roots.push_back(executablePath.parent_path());
    }

    for (fs::path root : roots) {
        for (int level = 0; level < 4 && !root.empty(); ++level) {
            const fs::path candidate = root / "data";
            if (fs::is_directory(candidate, error) && !error) {
                return candidate.lexically_normal().string();
            }
            error.clear();
            root = root.parent_path();
        }
    }

    // Le message de chargement indiquera clairement l'absence éventuelle des fichiers.
    return "data";
}

void printDevice(const myzone::MacAddress& mac, const myzone::DeviceInfo& info) {
    myzone::ui::title("RÉSULTAT  /  ADRESSE MAC");
    myzone::ui::log(myzone::ui::LogLevel::Info,
                    "Adresse MAC : " + myzone::ui::color(mac.toString(), myzone::ui::cyan));
    myzone::ui::log(myzone::ui::LogLevel::Info,
                    "Fabricant : " + myzone::ui::color(info.companyName, myzone::ui::bold));
    myzone::ui::log(myzone::ui::LogLevel::Info, "Catégorie : " + myzone::toString(info.category));
    myzone::ui::log(myzone::ui::LogLevel::Info, "Source : " + info.source);
}

void lookupMac(const myzone::Database& database) {
    myzone::ui::title("RECHERCHE  /  ADRESSE MAC");
    myzone::ui::log(myzone::ui::LogLevel::Info,
                    "Formats : AA:BB:CC:DD:EE:FF, AA-BB-CC-DD-EE-FF ou AABBCCDDEEFF");
    const std::string rawMac = myzone::ui::prompt("Adresse MAC (Entrée pour retour) : ");
    if (rawMac.empty()) {
        return;
    }

    try {
        const myzone::MacAddress mac(rawMac);
        myzone::DeviceInfo info;
        if (database.lookup(mac, info)) {
            printDevice(mac, info);
        } else {
            myzone::ui::warning("Adresse valide, mais aucun fabricant n'a été trouvé dans les sources OUI chargées.");
        }
    } catch (const std::invalid_argument& exception) {
        myzone::ui::error(exception.what());
    }
    myzone::ui::waitForEnter();
}

void printMenu() {
    myzone::ui::printMenu();
}

} // namespace

int main(int argc, char* argv[]) {
    myzone::ui::initConsole();
    myzone::ui::separator();
    const std::string dataDirectory = locateDataDirectory(argc > 0 ? argv[0] : "MyZone");
    myzone::ui::log(myzone::ui::LogLevel::Startup, "Démarrage de MyZone");
    myzone::ui::log(myzone::ui::LogLevel::Pending, "Chargement des référentiels réseau...");
    const myzone::Database database(dataDirectory);

    if (!database.isReady()) {
        myzone::ui::error("Aucune source OUI n'a été chargée. Vérifiez le dossier data/.");
        return 1;
    }

    myzone::ui::log(myzone::ui::LogLevel::Info, "Répertoire des données : " + database.dataDirectory());
    myzone::ui::success(std::to_string(database.vendorCount()) + " préfixes OUI indexés.");
    myzone::ui::success(std::to_string(database.dhcpFingerprintCount()) + " empreintes DHCP indexées.");

    for (;;) {
        printMenu();
        const std::string choice = myzone::ui::prompt("Votre choix : ");
        if (!std::cin) {
            std::cout << '\n';
            break;
        }

        if (choice == "1") {
            lookupMac(database);
        } else if (choice == "0" || choice == "q" || choice == "Q") {
            break;
        } else {
            myzone::ui::warning("Choix inconnu. Sélectionnez une option du tableau de bord.");
        }
    }

    myzone::ui::success("Arrêt de MyZone. À bientôt sur votre réseau.");
    myzone::ui::drawFooter();
    return 0;
}
