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
    std::cout << "  Adresse      " << myzone::ui::color(mac.toString(), myzone::ui::cyan) << '\n';
    std::cout << "  Fabricant    " << myzone::ui::color(info.companyName, myzone::ui::bold) << '\n';
    std::cout << "  Catégorie    " << myzone::toString(info.category) << '\n';
    std::cout << "  Source       " << info.source << "\n";
}

void lookupMac(const myzone::Database& database) {
    myzone::ui::title("RECHERCHE  /  ADRESSE MAC");
    std::cout << "  Formats acceptés : AA:BB:CC:DD:EE:FF, AA-BB-CC-DD-EE-FF ou AABBCCDDEEFF\n";
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
    const std::string dataDirectory = locateDataDirectory(argc > 0 ? argv[0] : "MyZone");
    const myzone::Database database(dataDirectory);

    if (!database.isReady()) {
        myzone::ui::error("Aucune source OUI n'a été chargée. Vérifiez le dossier data/.");
        return 1;
    }

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

    std::cout << "\n  MyZone arrêté. À bientôt sur votre réseau.\n\n";
    myzone::ui::drawFooter();
    return 0;
}
