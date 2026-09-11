#include <cstdlib>
#include <iostream>
#include <memory>
#include <stdexcept>
#include <string>
#include <vector>
#include <sys/stat.h>

#ifdef _WIN32
#include <direct.h>
#else
#include <unistd.h>
#endif

#include "include/Database.hpp"
#include "include/DeviceCategory.hpp"
#include "include/DeviceProfile.hpp"
#include "include/IdentificationEngine.hpp"
#include "include/MacAddress.hpp"
#include "include/modules/OuiModule.hpp"
#include "include/modules/DhcpModule.hpp"
#include "include/modules/HostnameModule.hpp"
#include "include/modules/P0fModule.hpp"
#include "include/modules/FingerBankModule.hpp"

#include "include/UI.hpp"

namespace {

bool isDirectory(const std::string& path) {
    struct stat info;
    if (stat(path.c_str(), &info) != 0) {
        return false;
    }
    return (info.st_mode & S_IFDIR) != 0;
}

std::string getCurrentDirectory() {
    char buffer[4096];
#ifdef _WIN32
    if (_getcwd(buffer, sizeof(buffer)) != nullptr) {
#else
    if (getcwd(buffer, sizeof(buffer)) != nullptr) {
#endif
        return std::string(buffer);
    }
    return ".";
}

std::string getParentDirectory(const std::string& path) {
    std::size_t pos = path.find_last_of("/\\");
    if (pos == std::string::npos) {
        return "";
    }
    return path.substr(0, pos);
}

std::string locateDataDirectory(const char* executable) {
    std::vector<std::string> roots;
    roots.push_back(getCurrentDirectory());

    if (executable != nullptr) {
        std::string execStr(executable);
        std::string execParent = getParentDirectory(execStr);
        if (!execParent.empty()) {
            roots.push_back(execParent);
        }
    }

    for (std::string root : roots) {
        for (int level = 0; level < 4 && !root.empty(); ++level) {
            std::string candidate = root;
            if (!candidate.empty() && candidate.back() != '/' && candidate.back() != '\\') {
                candidate += "/";
            }
            candidate += "data";

            if (isDirectory(candidate)) {
                return candidate;
            }
            root = getParentDirectory(root);
        }
    }

    return "data";
}

void printBanner() {
    myzone::ui::separator();
    std::cout << "  " << myzone::ui::color("MyZone v1.0", myzone::ui::bold) 
              << "  ·  " << myzone::ui::color("Network Discovery Engine", myzone::ui::cyan) << '\n';
    myzone::ui::separator();
}

void printMainMenu() {
    std::cout << '\n';
    std::cout << "   " << myzone::ui::color("1", myzone::ui::cyan) << "  Identifier un appareil (MAC, DHCP, hostname)\n";
    std::cout << "   " << myzone::ui::color("0", myzone::ui::red) << "  Quitter\n\n";
}

void printProfile(const myzone::DeviceProfile& profile) {
    myzone::ui::title("RÉSULTAT  /  IDENTIFICATION");

    if (profile.mac.isValid()) {
        myzone::ui::log(myzone::ui::LogLevel::Info,
                        "Adresse MAC : " + myzone::ui::color(profile.mac.toString(), myzone::ui::cyan));
    }
    if (!profile.manufacturer.empty()) {
        myzone::ui::log(myzone::ui::LogLevel::Info,
                        "Fabricant : " + myzone::ui::color(profile.manufacturer, myzone::ui::bold));
    }
    if (profile.category != myzone::DeviceCategory::Unknown) {
        myzone::ui::log(myzone::ui::LogLevel::Info,
                        "Catégorie : " + myzone::toString(profile.category));
    }
    if (!profile.osName.empty()) {
        myzone::ui::log(myzone::ui::LogLevel::Info,
                        "OS / Appareil : " + myzone::ui::color(profile.osName, myzone::ui::cyan));
    }
    if (!profile.deviceModel.empty() && profile.deviceModel != profile.osName) {
        myzone::ui::log(myzone::ui::LogLevel::Info,
                        "Modèle : " + myzone::ui::color(profile.deviceModel, myzone::ui::cyan));
    }

    if (!profile.results.empty()) {
        myzone::ui::log(myzone::ui::LogLevel::Pending, "Modules :");
        for (const auto& result : profile.results) {
            myzone::ui::log(myzone::ui::LogLevel::Info,
                            "  [" + result.moduleName + "] " + result.detail);
        }
    }
}

void identifyDevice(const myzone::IdentificationEngine& engine) {
    std::cout << '\n';
    std::cout << myzone::ui::color(" [ IDENTIFICATION ]", myzone::ui::bold) << "\n\n";
    myzone::ui::info("Fournissez les informations disponibles (Entrée pour ignorer)");
    myzone::ui::info("Formats MAC : AA:BB:CC:DD:EE:FF, AA-BB-CC-DD-EE-FF ou AABBCCDDEEFF\n");

    const std::string rawMac = myzone::ui::prompt("Adresse MAC : ");
    const std::string dhcpFp = myzone::ui::prompt("Empreinte DHCP (hash MD5) : ");
    const std::string hostname = myzone::ui::prompt("Hostname : ");
    const std::string tcpSig = myzone::ui::prompt("Signature TCP (p0f) : ");
    const std::string dhcpOpts = myzone::ui::prompt("Options DHCP (liste, ex: 1,15,3,6) : ");

    if (rawMac.empty() && dhcpFp.empty() && hostname.empty() && tcpSig.empty() && dhcpOpts.empty()) {
        myzone::ui::warning("Aucune donnée fournie.");
        return;
    }

    myzone::DeviceProfile input;

    if (!rawMac.empty()) {
        try {
            input.mac = myzone::MacAddress(rawMac);
        } catch (const std::invalid_argument& e) {
            myzone::ui::error(e.what());
            myzone::ui::waitForEnter();
            return;
        }
    }

    input.dhcpFingerprint = dhcpFp;
    input.hostname = hostname;
    input.tcpSignature = tcpSig;
    input.dhcpOptions = dhcpOpts;

    const myzone::DeviceProfile result = engine.identify(input);

    if (!result.hasResults()) {
        myzone::ui::warning("Aucun module n'a pu identifier cet appareil.");
    } else {
        printProfile(result);
    }

    myzone::ui::waitForEnter();
}

} // namespace

int main(int argc, char* argv[]) {
    myzone::ui::initConsole();
    printBanner();

    const std::string dataDirectory = locateDataDirectory(argc > 0 ? argv[0] : "MyZone");
    myzone::ui::log(myzone::ui::LogLevel::Startup, "Démarrage de MyZone");
    myzone::ui::log(myzone::ui::LogLevel::Pending, "Chargement des référentiels réseau...");
    
    myzone::ui::info("Répertoire des données : " + dataDirectory);
    const myzone::Database database(dataDirectory);

    if (!database.isReady()) {
        myzone::ui::error("Aucune source OUI n'a été chargée. Vérifiez le dossier data/.");
        return 1;
    }

    myzone::ui::success(std::to_string(database.vendorCount()) + " préfixes OUI indexés");
    myzone::ui::success(std::to_string(database.dhcpFingerprintCount()) + " empreintes DHCP indexées");
    myzone::ui::success(std::to_string(database.p0fSignatureCount()) + " signatures TCP p0f indexées");
    myzone::ui::success(std::to_string(database.fingerBankCount()) + " listes d'options FingerBank indexées");

    myzone::IdentificationEngine engine;
    engine.addModule(std::make_unique<myzone::OuiModule>(database));
    engine.addModule(std::make_unique<myzone::DhcpModule>(database));
    engine.addModule(std::make_unique<myzone::HostnameModule>());
    engine.addModule(std::make_unique<myzone::P0fModule>(database));
    engine.addModule(std::make_unique<myzone::FingerBankModule>(database));

    myzone::ui::success(std::to_string(engine.moduleCount()) + " modules d'identification actifs");

    bool firstMenu = true;
    while (true) {
        if (!firstMenu) {
            myzone::ui::clearConsole();
            printBanner();
        }
        firstMenu = false;
        
        printMainMenu();
        const std::string choice = myzone::ui::prompt("Votre choix : ");
        if (!std::cin) {
            std::cout << '\n';
            break;
        }

        if (choice == "1") {
            myzone::ui::clearConsole();
            printBanner();
            printMainMenu();
            std::cout << " " << myzone::ui::color("➤", myzone::ui::cyan) << " Votre choix : 1\n";
            identifyDevice(engine);
        } else if (choice == "0" || choice == "q" || choice == "Q") {
            break;
        } else {
            myzone::ui::warning("Choix inconnu.");
        }
    }

    std::cout << '\n';
    myzone::ui::separator();
    myzone::ui::log(myzone::ui::LogLevel::Complete, "Session MyZone terminée.");
    return 0;
}
