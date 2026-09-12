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
    std::cout << "   " << myzone::ui::color("2", myzone::ui::cyan) << "  Rechercher une empreinte DHCP connue\n";
    std::cout << "   " << myzone::ui::color("3", myzone::ui::cyan) << "  Voir les données chargées et les signatures\n";
    std::cout << "   " << myzone::ui::color("4", myzone::ui::cyan) << "  Lancer une démonstration avec des MAC d'exemple\n";
    std::cout << "   " << myzone::ui::color("5", myzone::ui::cyan) << "  Afficher l'aide et les limites d'identification\n";
    std::cout << "   " << myzone::ui::color("0", myzone::ui::red)  << "  Quitter\n\n";
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
                            "[" + result.moduleName + "]" + result.detail);
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

void searchDhcp(const myzone::Database& database) {
    std::cout << '\n';
    std::cout << myzone::ui::color(" [ RECHERCHE DHCP ]", myzone::ui::bold) << "\n\n";
    myzone::ui::info("Entrez un hash MD5 d'empreinte DHCP (32 caractères hexadécimaux).\n");

    const std::string hash = myzone::ui::prompt("Hash MD5 : ");
    if (hash.empty()) {
        myzone::ui::warning("Aucune empreinte fournie.");
        myzone::ui::waitForEnter();
        return;
    }

    myzone::DhcpFingerprintInfo info;
    if (database.lookupDhcpFingerprint(hash, info)) {
        myzone::ui::title("RÉSULTAT  /  EMPREINTE DHCP");
        myzone::ui::log(myzone::ui::LogLevel::Info,
                        "Empreinte : " + myzone::ui::color(info.fingerprint, myzone::ui::cyan));
        myzone::ui::log(myzone::ui::LogLevel::Info,
                        "Appareil  : " + myzone::ui::color(info.deviceName, myzone::ui::bold));
        if (!info.requestedOptions.empty()) {
            myzone::ui::log(myzone::ui::LogLevel::Info,
                            "Options   : " + info.requestedOptions);
        }
        myzone::ui::log(myzone::ui::LogLevel::Info,
                        "Confiance : " + std::to_string(info.confidence) + " %");
    } else {
        myzone::ui::warning("Aucune correspondance trouvée pour ce hash.");
    }

    myzone::ui::waitForEnter();
}

void showSources(const myzone::Database& database) {
    std::cout << '\n';
    std::cout << myzone::ui::color(" [ CATALOGUE DES SOURCES ]", myzone::ui::bold) << "\n\n";

    myzone::ui::log(myzone::ui::LogLevel::Info,
                    "Répertoire des données : " + myzone::ui::color(database.dataDirectory(), myzone::ui::cyan));
    myzone::ui::log(myzone::ui::LogLevel::Info,
                    std::to_string(database.vendorCount()) + " préfixes OUI uniques indexés");
    myzone::ui::log(myzone::ui::LogLevel::Info,
                    std::to_string(database.dhcpFingerprintCount()) + " empreintes DHCP indexées");
    myzone::ui::log(myzone::ui::LogLevel::Info,
                    std::to_string(database.p0fSignatureCount()) + " signatures TCP p0f indexées");
    myzone::ui::log(myzone::ui::LogLevel::Info,
                    std::to_string(database.fingerBankCount()) + " listes d'options FingerBank indexées");

    std::cout << '\n';
    myzone::ui::log(myzone::ui::LogLevel::Pending, "Détail par source :");
    std::cout << '\n';

    const auto& sources = database.sources();
    for (const auto& source : sources) {
        const std::string status = source.available
            ? (source.indexed
                ? myzone::ui::color("INDEXÉ", myzone::ui::green)
                : myzone::ui::color("DISPONIBLE", myzone::ui::yellow))
            : myzone::ui::color("ABSENT", myzone::ui::red);

        myzone::ui::log(myzone::ui::LogLevel::Info,
                        myzone::ui::color(source.fileName, myzone::ui::bold));
        myzone::ui::log(myzone::ui::LogLevel::Pending,
                        "  Label   : " + source.label);
        myzone::ui::log(myzone::ui::LogLevel::Pending,
                        "  Rôle    : " + source.purpose);
        myzone::ui::log(myzone::ui::LogLevel::Pending,
                        "  État    : " + status
                        + (source.records > 0 ? ("  (" + std::to_string(source.records) + " entrées)") : ""));
        std::cout << '\n';
    }

    myzone::ui::waitForEnter();
}

void runDemo(const myzone::IdentificationEngine& engine) {
    std::cout << '\n';
    std::cout << myzone::ui::color(" [ DÉMONSTRATION ]", myzone::ui::bold) << "\n\n";
    myzone::ui::info("Lancement de l'identification sur des exemples connus.\n");

    struct DemoEntry {
        std::string label;
        std::string mac;
        std::string hostname;
        std::string dhcpOptions;
    };

    const std::vector<DemoEntry> demos = {
        {"Apple iPhone (via MAC)",             "AC:DE:48:00:11:22", "",                   ""},
        {"Samsung Galaxy (via hostname)",      "00:00:00:00:00:00", "Galaxy-S23-Ultra",   ""},
        {"Cisco routeur (via MAC)",            "00:1A:2B:00:00:00", "",                   ""},
        {"Windows PC (via hostname)",          "",                  "DESKTOP-AB1CD2E",    ""},
        {"Google Pixel (via hostname)",        "",                  "Pixel-8-Pro",        ""},
        {"MacBook (via hostname)",             "",                  "MacBook-Pro-de-Tom", ""},
        {"iPad (via hostname)",                "",                  "iPad-de-Marie",      ""},
        {"Raspberry Pi (via MAC)",             "B8:27:EB:12:34:56", "",                   ""},
        {"TP-Link appareil réseau (via MAC)",  "50:C7:BF:AA:BB:CC", "",                   ""},
        {"Intel device (via MAC)",             "00:1B:21:00:00:00", "",                   ""},
    };

    for (std::size_t i = 0; i < demos.size(); ++i) {
        const auto& demo = demos[i];
        myzone::ui::separator();
        myzone::ui::log(myzone::ui::LogLevel::Startup,
                        "Exemple " + std::to_string(i + 1) + "/" + std::to_string(demos.size())
                        + " : " + myzone::ui::color(demo.label, myzone::ui::cyan));

        myzone::DeviceProfile input;
        if (!demo.mac.empty() && demo.mac != "00:00:00:00:00:00") {
            try {
                input.mac = myzone::MacAddress(demo.mac);
            } catch (const std::invalid_argument&) {
                // Ignore invalid demo MACs
            }
        }
        input.hostname = demo.hostname;
        input.dhcpOptions = demo.dhcpOptions;

        const myzone::DeviceProfile result = engine.identify(input);

        if (!result.hasResults()) {
            myzone::ui::warning("Aucun résultat pour cet exemple.");
        } else {
            printProfile(result);
        }
        std::cout << '\n';
    }

    myzone::ui::waitForEnter();
}

void showHelp() {
    std::cout << '\n';
    std::cout << myzone::ui::color(" [ AIDE  /  LIMITES D'IDENTIFICATION ]", myzone::ui::bold) << "\n\n";

    myzone::ui::title("QU'EST-CE QUE MYZONE ?");
    myzone::ui::info("MyZone est un outil d'identification locale d'appareils réseau.");
    myzone::ui::info("Il combine plusieurs sources de données pour identifier un appareil");
    myzone::ui::info("à partir de son adresse MAC, son hostname, son empreinte DHCP ou");
    myzone::ui::info("sa signature TCP.\n");

    myzone::ui::title("MODULES D'IDENTIFICATION");
    myzone::ui::log(myzone::ui::LogLevel::Success,
                    myzone::ui::color("OUI", myzone::ui::bold)
                    + "         Identifie le fabricant via le préfixe MAC (OUI).");
    myzone::ui::log(myzone::ui::LogLevel::Success,
                    myzone::ui::color("DHCP", myzone::ui::bold)
                    + "        Recherche d'un appareil par hash MD5 d'empreinte DHCP.");
    myzone::ui::log(myzone::ui::LogLevel::Success,
                    myzone::ui::color("Hostname", myzone::ui::bold)
                    + "    Détecte l'OS et le type d'appareil à partir du hostname.");
    myzone::ui::log(myzone::ui::LogLevel::Success,
                    myzone::ui::color("p0f TCP", myzone::ui::bold)
                    + "     Identifie l'OS via les signatures TCP passives (p0f).");
    myzone::ui::log(myzone::ui::LogLevel::Success,
                    myzone::ui::color("FingerBank", myzone::ui::bold)
                    + "  Recherche l'OS par la liste d'options DHCP demandées.\n");

    myzone::ui::title("FORMATS ACCEPTÉS");
    myzone::ui::info("Adresse MAC   :  AA:BB:CC:DD:EE:FF  ou  AA-BB-CC-DD-EE-FF  ou  AABBCCDDEEFF");
    myzone::ui::info("Hash DHCP     :  Chaîne de 32 caractères hexadécimaux (MD5)");
    myzone::ui::info("Hostname      :  Nom réseau de l'appareil (ex: DESKTOP-AB1CD2E, iPhone-de-Tom)");
    myzone::ui::info("Signature TCP :  Signature au format p0f (ex: *:64:0:*:mss*20,7:mss,sok,...)");
    myzone::ui::info("Options DHCP  :  Liste d'options séparées par des virgules (ex: 1,15,3,6,44)\n");

    myzone::ui::title("LIMITES");
    myzone::ui::warning("Une adresse MAC identifie un bloc attribué à un fabricant, pas un modèle exact.");
    myzone::ui::warning("Certains appareils usurpent (spoof) leur MAC : l'identification peut être fausse.");
    myzone::ui::warning("Le hostname peut être changé manuellement par l'utilisateur.");
    myzone::ui::warning("Les modules fonctionnent par heuristique, pas par certitude absolue.");
    myzone::ui::warning("MyZone ne scanne pas le réseau et n'envoie aucune donnée.\n");

    myzone::ui::title("SOURCES DE DONNÉES");
    myzone::ui::info("Les bases proviennent de projets open-source reconnus :");
    myzone::ui::log(myzone::ui::LogLevel::Pending, "  • IEEE OUI via lookup.csv (référentiel principal)");
    myzone::ui::log(myzone::ui::LogLevel::Pending, "  • Wireshark manuf (compléments fabricants)");
    myzone::ui::log(myzone::ui::LogLevel::Pending, "  • Nmap mac-prefixes (compléments fabricants)");
    myzone::ui::log(myzone::ui::LogLevel::Pending, "  • KYD / FingerBank (empreintes DHCP)");
    myzone::ui::log(myzone::ui::LogLevel::Pending, "  • p0f (signatures TCP/IP passives)");
    myzone::ui::log(myzone::ui::LogLevel::Pending, "  • FingerBank dhcp_fingerprints.conf (options DHCP)");
    myzone::ui::log(myzone::ui::LogLevel::Pending, "  • Nmap OS DB et service probes (disponibles, non indexés)\n");

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
            identifyDevice(engine);
        } else if (choice == "2") {
            myzone::ui::clearConsole();
            printBanner();
            searchDhcp(database);
        } else if (choice == "3") {
            myzone::ui::clearConsole();
            printBanner();
            showSources(database);
        } else if (choice == "4") {
            myzone::ui::clearConsole();
            printBanner();
            runDemo(engine);
        } else if (choice == "5") {
            myzone::ui::clearConsole();
            printBanner();
            showHelp();
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
