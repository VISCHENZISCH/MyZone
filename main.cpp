#include <cstdlib>
#include <iostream>
#include <iomanip>
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

void printMainMenu() {
    std::cout << '\n';
    std::cout << myzone::ui::tagSuccess() << " " << myzone::ui::color("Options disponibles :", myzone::ui::whiteBright) << "\n\n";
    std::cout << "  " << myzone::ui::color("[1]", myzone::ui::greenBright) << " Identifier un appareil (MAC, DHCP, hostname, TCP)\n";
    std::cout << "  " << myzone::ui::color("[2]", myzone::ui::greenBright) << " Rechercher une empreinte DHCP connue\n";
    std::cout << "  " << myzone::ui::color("[3]", myzone::ui::greenBright) << " Voir le catalogue des sources et signatures\n";
    std::cout << "  " << myzone::ui::color("[4]", myzone::ui::greenBright) << " Lancer une demonstration avec des cibles d'exemple\n";
    std::cout << "  " << myzone::ui::color("[5]", myzone::ui::greenBright) << " Afficher l'aide et les limites d'identification\n";
    std::cout << "  " << myzone::ui::color("[0]", myzone::ui::redBright)   << " Quitter\n\n";
}

void printProfile(const myzone::DeviceProfile& profile) {
    std::cout << '\n';
    std::cout << myzone::ui::tagSuccess() << " " << myzone::ui::color("Resultat d'identification :", myzone::ui::whiteBright) << '\n';
    myzone::ui::divider('-');

    if (profile.mac.isValid()) {
        std::cout << "    " << myzone::ui::color("Adresse MAC   : ", myzone::ui::dim)
                  << myzone::ui::color(profile.mac.toString(), myzone::ui::greenBright) << '\n';
    }
    if (!profile.manufacturer.empty()) {
        std::cout << "    " << myzone::ui::color("Fabricant     : ", myzone::ui::dim)
                  << myzone::ui::color(profile.manufacturer, myzone::ui::whiteBright) << '\n';
    }
    if (profile.category != myzone::DeviceCategory::Unknown) {
        std::cout << "    " << myzone::ui::color("Categorie     : ", myzone::ui::dim)
                  << myzone::ui::color(myzone::toString(profile.category), myzone::ui::cyanBright) << '\n';
    }
    if (!profile.osName.empty()) {
        std::cout << "    " << myzone::ui::color("OS / Appareil : ", myzone::ui::dim)
                  << myzone::ui::color(profile.osName, myzone::ui::yellowBright) << '\n';
    }
    if (!profile.deviceModel.empty() && profile.deviceModel != profile.osName) {
        std::cout << "    " << myzone::ui::color("Modele        : ", myzone::ui::dim)
                  << myzone::ui::color(profile.deviceModel, myzone::ui::cyanBright) << '\n';
    }

    if (!profile.results.empty()) {
        std::cout << '\n';
        std::cout << "    " << myzone::ui::color("Modules actifs :", myzone::ui::whiteBright) << '\n';
        for (const auto& result : profile.results) {
            std::cout << "    " << myzone::ui::tagInfo() << " ["
                      << myzone::ui::color(result.moduleName, myzone::ui::greenBright) << "] "
                      << result.detail << '\n';
        }
    }
    myzone::ui::divider('-');
}

void identifyDevice(const myzone::IdentificationEngine& engine) {
    std::cout << '\n';
    std::cout << myzone::ui::tagSuccess() << " " << myzone::ui::color("Mode Identification d'appareil", myzone::ui::whiteBright) << '\n';
    myzone::ui::divider('-');
    std::cout << "  " << myzone::ui::tagInfo() << " Renseignez les elements connus (appuyez sur Entree pour ignorer)\n";
    std::cout << "  " << myzone::ui::tagInfo() << " Formats MAC : AA:BB:CC:DD:EE:FF | AA-BB-CC-DD-EE-FF | AABBCCDDEEFF\n\n";

    const std::string rawMac   = myzone::ui::prompt("Adresse MAC              : ");
    const std::string dhcpFp   = myzone::ui::prompt("Empreinte DHCP (hash MD5): ");
    const std::string hostname = myzone::ui::prompt("Nom d'hote (Hostname)    : ");
    const std::string tcpSig   = myzone::ui::prompt("Signature TCP (p0f)      : ");
    const std::string dhcpOpts = myzone::ui::prompt("Options DHCP (ex: 1,3,6) : ");

    if (rawMac.empty() && dhcpFp.empty() && hostname.empty() && tcpSig.empty() && dhcpOpts.empty()) {
        std::cout << '\n';
        myzone::ui::warning("Aucune donnee fournie pour l'identification.");
        myzone::ui::waitForEnter();
        return;
    }

    myzone::DeviceProfile input;

    if (!rawMac.empty()) {
        try {
            input.mac = myzone::MacAddress(rawMac);
        } catch (const std::invalid_argument& e) {
            std::cout << '\n';
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
        std::cout << '\n';
        myzone::ui::warning("Aucun module n'a pu identifier cet appareil avec les elements fournis.");
    } else {
        printProfile(result);
    }

    myzone::ui::waitForEnter();
}

void searchDhcp(const myzone::Database& database) {
    std::cout << '\n';
    std::cout << myzone::ui::tagSuccess() << " " << myzone::ui::color("Recherche d'empreinte DHCP (KYD / FingerBank)", myzone::ui::whiteBright) << '\n';
    myzone::ui::divider('-');
    std::cout << "  " << myzone::ui::tagInfo() << " Entrez un hash MD5 d'empreinte DHCP (32 caracteres hexadecimaux)\n\n";

    const std::string hash = myzone::ui::prompt("Hash MD5 : ");
    if (hash.empty()) {
        std::cout << '\n';
        myzone::ui::warning("Aucune empreinte fournie.");
        myzone::ui::waitForEnter();
        return;
    }

    myzone::DhcpFingerprintInfo info;
    std::cout << '\n';
    if (database.lookupDhcpFingerprint(hash, info)) {
        std::cout << myzone::ui::tagSuccess() << " " << myzone::ui::color("Correspondance DHCP identifiee :", myzone::ui::whiteBright) << '\n';
        myzone::ui::divider('-');
        std::cout << "    " << myzone::ui::color("Empreinte : ", myzone::ui::dim)
                  << myzone::ui::color(info.fingerprint, myzone::ui::cyanBright) << '\n';
        std::cout << "    " << myzone::ui::color("Appareil  : ", myzone::ui::dim)
                  << myzone::ui::color(info.deviceName, myzone::ui::whiteBright) << '\n';
        if (!info.requestedOptions.empty()) {
            std::cout << "    " << myzone::ui::color("Options   : ", myzone::ui::dim)
                      << info.requestedOptions << '\n';
        }
        std::cout << "    " << myzone::ui::color("Confiance : ", myzone::ui::dim)
                  << myzone::ui::color(std::to_string(info.confidence) + " %", myzone::ui::greenBright) << '\n';
        myzone::ui::divider('-');
    } else {
        myzone::ui::warning("Aucune correspondance trouvee pour ce hash MD5.");
    }

    myzone::ui::waitForEnter();
}

std::string padRight(const std::string& str, std::size_t width) {
    if (str.size() >= width) return str;
    return str + std::string(width - str.size(), ' ');
}

std::string padLeft(const std::string& str, std::size_t width) {
    if (str.size() >= width) return str;
    return std::string(width - str.size(), ' ') + str;
}

void showSources(const myzone::Database& database) {
    std::cout << '\n';
    std::cout << myzone::ui::tagSuccess() << " " << myzone::ui::color("Catalogue des sources & bases locales", myzone::ui::whiteBright) << '\n';
    myzone::ui::divider('-');
    std::cout << "  " << myzone::ui::tagInfo() << " Repertoire des donnees : "
              << myzone::ui::color(database.dataDirectory(), myzone::ui::cyanBright) << '\n';
    std::cout << "  " << myzone::ui::tagInfo() << " Total indexes : "
              << myzone::ui::color(std::to_string(database.vendorCount()), myzone::ui::greenBright) << " OUI  |  "
              << myzone::ui::color(std::to_string(database.dhcpFingerprintCount()), myzone::ui::greenBright) << " DHCP  |  "
              << myzone::ui::color(std::to_string(database.p0fSignatureCount()), myzone::ui::greenBright) << " p0f TCP  |  "
              << myzone::ui::color(std::to_string(database.fingerBankCount()), myzone::ui::greenBright) << " FingerBank\n\n";

    // En-têtes du tableau style wifite2
    std::cout << "  "
              << myzone::ui::color("FICHIER", myzone::ui::greenBright)
              << std::string(24 - 7, ' ')
              << myzone::ui::color("ENTREES", myzone::ui::greenBright)
              << "   "
              << myzone::ui::color("STATUT", myzone::ui::greenBright)
              << std::string(12 - 6, ' ')
              << myzone::ui::color("ROLE / DESCRIPTION", myzone::ui::greenBright)
              << '\n';

    std::cout << "  "
              << std::string(22, '-') << "  "
              << std::string(8, '-')  << "  "
              << std::string(10, '-') << "  "
              << std::string(40, '-') << '\n';

    const auto& sources = database.sources();
    for (const auto& source : sources) {
        std::string statusText = source.available
            ? (source.indexed ? "INDEXE" : "DISPONIBLE")
            : "ABSENT";

        const char* statusColor = source.available
            ? (source.indexed ? myzone::ui::greenBright : myzone::ui::yellowBright)
            : myzone::ui::redBright;

        std::string recordsStr = source.records > 0 ? std::to_string(source.records) : "-";

        std::cout << "  "
                  << padRight(source.fileName, 22) << "  "
                  << padLeft(recordsStr, 8) << "  "
                  << myzone::ui::color(padRight(statusText, 10), statusColor) << "  "
                  << source.purpose << '\n';
    }

    std::cout << '\n';
    myzone::ui::waitForEnter();
}

void runDemo(const myzone::IdentificationEngine& engine) {
    std::cout << '\n';
    std::cout << myzone::ui::tagSuccess() << " " << myzone::ui::color("Demonstration sur des cibles d'exemple", myzone::ui::whiteBright) << '\n';
    myzone::ui::divider('-');
    std::cout << "  " << myzone::ui::tagInfo() << " Evaluation automatique de 10 configurations d'appareils\n\n";

    struct DemoEntry {
        std::string label;
        std::string mac;
        std::string hostname;
        std::string dhcpOptions;
    };

    const std::vector<DemoEntry> demos = {
        {"Apple iPhone (via MAC)",             "AC:DE:48:00:11:22", "",                   ""},
        {"Samsung Galaxy (via hostname)",      "",                  "Galaxy-S23-Ultra",   ""},
        {"Cisco routeur (via MAC)",            "00:1A:2B:00:00:00", "",                   ""},
        {"Windows PC (via hostname)",          "",                  "DESKTOP-AB1CD2E",    ""},
        {"Google Pixel (via hostname)",        "",                  "Pixel-8-Pro",        ""},
        {"MacBook (via hostname)",             "",                  "MacBook-Pro-de-Tom", ""},
        {"iPad (via hostname)",                "",                  "iPad-de-Marie",      ""},
        {"Raspberry Pi (via MAC)",             "B8:27:EB:12:34:56", "",                   ""},
        {"TP-Link appareil (via MAC)",         "50:C7:BF:AA:BB:CC", "",                   ""},
        {"Intel device (via MAC)",             "00:1B:21:00:00:00", "",                   ""},
    };

    // Tableau résumé style hacking
    std::cout << "  "
              << myzone::ui::color("NUM", myzone::ui::greenBright) << "   "
              << myzone::ui::color("CIBLE", myzone::ui::greenBright) << std::string(32 - 5, ' ')
              << myzone::ui::color("ENTREE", myzone::ui::greenBright) << std::string(22 - 6, ' ')
              << myzone::ui::color("RESULTAT / IDENTIFICATION", myzone::ui::greenBright) << '\n';

    std::cout << "  "
              << std::string(4, '-')  << "  "
              << std::string(30, '-') << "  "
              << std::string(20, '-') << "  "
              << std::string(36, '-') << '\n';

    for (std::size_t i = 0; i < demos.size(); ++i) {
        const auto& demo = demos[i];
        myzone::DeviceProfile input;

        std::string testValue;
        if (!demo.mac.empty()) {
            try { input.mac = myzone::MacAddress(demo.mac); testValue = demo.mac; } catch (...) {}
        }
        if (!demo.hostname.empty()) {
            input.hostname = demo.hostname;
            if (testValue.empty()) testValue = demo.hostname;
        }

        const myzone::DeviceProfile result = engine.identify(input);

        std::string identification;
        if (!result.manufacturer.empty()) {
            identification = result.manufacturer;
        }
        if (result.category != myzone::DeviceCategory::Unknown) {
            if (!identification.empty()) identification += " [";
            identification += myzone::toString(result.category);
            if (!result.manufacturer.empty()) identification += "]";
        } else if (!result.osName.empty()) {
            if (!identification.empty()) identification += " [";
            identification += result.osName;
            if (!result.manufacturer.empty()) identification += "]";
        }

        if (identification.empty()) {
            identification = myzone::ui::color("Inconnu", myzone::ui::dim);
        }

        std::string numStr = "[" + std::to_string(i + 1) + "]";
        std::cout << "  "
                  << myzone::ui::color(padRight(numStr, 4), myzone::ui::greenBright) << "  "
                  << padRight(demo.label, 30) << "  "
                  << myzone::ui::color(padRight(testValue, 20), myzone::ui::cyanBright) << "  "
                  << identification << '\n';
    }

    std::cout << '\n';
    myzone::ui::waitForEnter();
}

void showHelp() {
    std::cout << '\n';
    std::cout << myzone::ui::tagSuccess() << " " << myzone::ui::color("Aide & Limites d'identification MyZone", myzone::ui::whiteBright) << '\n';
    myzone::ui::divider('-');

    std::cout << '\n' << "  " << myzone::ui::color("MODULES D'IDENTIFICATION :", myzone::ui::greenBright) << '\n';
    std::cout << "    " << myzone::ui::tagSuccess() << " " << myzone::ui::color("OUI", myzone::ui::whiteBright)
              << "         Identifie le constructeur via le prefixe MAC (IEEE / Wireshark / Nmap)\n";
    std::cout << "    " << myzone::ui::tagSuccess() << " " << myzone::ui::color("DHCP", myzone::ui::whiteBright)
              << "        Recherche le modele et l'OS par hash MD5 d'options DHCP (KYD / FingerBank)\n";
    std::cout << "    " << myzone::ui::tagSuccess() << " " << myzone::ui::color("Hostname", myzone::ui::whiteBright)
              << "    Analyse heuristique du nom d'hote reseau (ex: Apple, Samsung, Windows)\n";
    std::cout << "    " << myzone::ui::tagSuccess() << " " << myzone::ui::color("p0f TCP", myzone::ui::whiteBright)
              << "     Identification passive de l'OS par signature SYN TCP/IP (p0f v3)\n";
    std::cout << "    " << myzone::ui::tagSuccess() << " " << myzone::ui::color("FingerBank", myzone::ui::whiteBright)
              << "  Identification de l'OS via la liste ordonnee d'options DHCP demandees\n";

    std::cout << '\n' << "  " << myzone::ui::color("FORMATS ACCEPTES :", myzone::ui::greenBright) << '\n';
    std::cout << "    " << myzone::ui::tagInfo() << " Adresse MAC   : AA:BB:CC:DD:EE:FF | AA-BB-CC-DD-EE-FF | AABBCCDDEEFF\n";
    std::cout << "    " << myzone::ui::tagInfo() << " Hash DHCP     : Chaine hexadecimale de 32 caracteres (MD5)\n";
    std::cout << "    " << myzone::ui::tagInfo() << " Hostname      : Nom machine NetBIOS / mDNS (ex: DESKTOP-AB1CD2E, iPhone-de-Tom)\n";
    std::cout << "    " << myzone::ui::tagInfo() << " Signature TCP : Signature au format p0f (ex: *:64:0:*:mss*20,7:mss,sok,...)\n";
    std::cout << "    " << myzone::ui::tagInfo() << " Options DHCP  : Liste d'options separees par virgules (ex: 1,15,3,6,44)\n";

    std::cout << '\n' << "  " << myzone::ui::color("LIMITES & CONDITIONS D'AUDIT :", myzone::ui::yellowBright) << '\n';
    std::cout << "    " << myzone::ui::tagWarning() << " L'adresse MAC identifie un bloc attribue a un constructeur, pas un modele unique.\n";
    std::cout << "    " << myzone::ui::tagWarning() << " Les adresses MAC randomisees (Wi-Fi prive iOS/Android) masquent le fabricant officiel.\n";
    std::cout << "    " << myzone::ui::tagWarning() << " Le nom d'hote (hostname) peut etre modifie arbitrairement par l'utilisateur.\n";
    std::cout << "    " << myzone::ui::tagWarning() << " Les modules fonctionnent par heuristique et correlations passives.\n";
    std::cout << "    " << myzone::ui::tagWarning() << " MyZone est 100% passif et local : aucune trame n'est envoyee sur le reseau.\n\n";

    myzone::ui::waitForEnter();
}

struct CliArgs {
    std::string mac;
    std::string hostname;
    std::string dhcpFp;
    std::string tcpSig;
    std::string dhcpOpts;
    bool json = false;
    bool help = false;
    bool version = false;
    bool hasIdArg = false;
};

CliArgs parseArgs(int argc, char* argv[]) {
    CliArgs args;
    for (int i = 1; i < argc; ++i) {
        std::string arg = argv[i];
        if (arg == "--help" || arg == "-h") args.help = true;
        else if (arg == "--version" || arg == "-v") args.version = true;
        else if (arg == "--json") args.json = true;
        else if (arg == "--mac" && i + 1 < argc) { args.mac = argv[++i]; args.hasIdArg = true; }
        else if (arg == "--hostname" && i + 1 < argc) { args.hostname = argv[++i]; args.hasIdArg = true; }
        else if (arg == "--dhcp" && i + 1 < argc) { args.dhcpFp = argv[++i]; args.hasIdArg = true; }
        else if (arg == "--tcp" && i + 1 < argc) { args.tcpSig = argv[++i]; args.hasIdArg = true; }
        else if (arg == "--dhcp-options" && i + 1 < argc) { args.dhcpOpts = argv[++i]; args.hasIdArg = true; }
    }
    return args;
}

void printCliHelp() {
    myzone::ui::banner();
    std::cout << "Usage:\n"
              << "  MyZone                                      Mode interactif console\n"
              << "  MyZone --mac AA:BB:CC:DD:EE:FF [options]    Identification directe en ligne de commande\n\n"
              << "Options:\n"
              << "  --mac <address>        Adresse MAC de l'appareil\n"
              << "  --hostname <name>      Nom d'hote reseau\n"
              << "  --dhcp <hash>          Hash MD5 de signature DHCP\n"
              << "  --tcp <signature>      Signature TCP (p0f)\n"
              << "  --dhcp-options <list>  Options DHCP (ex: 1,15,3,6)\n"
              << "  --json                 Sortie stricte au format JSON (scriptable)\n"
              << "  --help, -h             Afficher cette aide\n"
              << "  --version, -v          Afficher la version\n";
}

} // namespace

int main(int argc, char* argv[]) {
    myzone::ui::initConsole();

    // 1. Analyse précoce des arguments de ligne de commande
    CliArgs args = parseArgs(argc, argv);

    if (args.help) {
        printCliHelp();
        return 0;
    }
    if (args.version) {
        std::cout << "MyZone v1.0\n";
        return 0;
    }

    // 2. En mode JSON, désactiver tous les messages et bannières textuels
    if (args.json) {
        myzone::ui::setQuiet(true);
    } else if (!args.hasIdArg) {
        // En mode interactif complet, afficher la bannière
        myzone::ui::banner();
    }

    // 3. Chargement des référentiels
    const std::string dataDirectory = locateDataDirectory(argc > 0 ? argv[0] : "MyZone");
    if (!args.json) {
        std::cout << myzone::ui::tagInfo() << " Chargement des bases de signatures ("
                  << myzone::ui::color(dataDirectory, myzone::ui::cyanBright) << ")...\n";
    }

    const myzone::Database database(dataDirectory);

    if (!database.isReady()) {
        myzone::ui::error("Aucune source OUI n'a ete chargee. Verifiez le dossier data/.");
        return 1;
    }

    myzone::IdentificationEngine engine;
    engine.addModule(std::make_unique<myzone::OuiModule>(database));
    engine.addModule(std::make_unique<myzone::DhcpModule>(database));
    engine.addModule(std::make_unique<myzone::HostnameModule>());
    engine.addModule(std::make_unique<myzone::P0fModule>(database));
    engine.addModule(std::make_unique<myzone::FingerBankModule>(database));

    if (!args.json && !args.hasIdArg) {
        std::cout << myzone::ui::tagSuccess() << " " << database.vendorCount() << " prefixes OUI indexes\n";
        std::cout << myzone::ui::tagSuccess() << " " << database.dhcpFingerprintCount() << " empreintes DHCP indexees\n";
        std::cout << myzone::ui::tagSuccess() << " " << database.p0fSignatureCount() << " signatures TCP p0f indexees\n";
        std::cout << myzone::ui::tagSuccess() << " " << database.fingerBankCount() << " listes d'options FingerBank indexees\n";
        std::cout << myzone::ui::tagSuccess() << " " << engine.moduleCount() << " modules d'identification prets\n";
    }

    // 4. Exécution directe CLI avec arguments
    if (args.hasIdArg) {
        myzone::DeviceProfile input;
        if (!args.mac.empty()) {
            try { input.mac = myzone::MacAddress(args.mac); } catch (...) {}
        }
        input.hostname = args.hostname;
        input.dhcpFingerprint = args.dhcpFp;
        input.tcpSignature = args.tcpSig;
        input.dhcpOptions = args.dhcpOpts;

        const myzone::DeviceProfile result = engine.identify(input);
        if (args.json) {
            std::cout << result.toJson() << "\n";
        } else {
            printProfile(result);
        }
        return 0;
    }

    // 5. Boucle interactive principale
    bool firstMenu = true;
    while (true) {
        if (!firstMenu) {
            myzone::ui::clearConsole();
            myzone::ui::banner();
        }
        firstMenu = false;

        printMainMenu();
        const std::string choice = myzone::ui::prompt("Choix : ");
        if (!std::cin) {
            std::cout << '\n';
            break;
        }

        if (choice == "1") {
            myzone::ui::clearConsole();
            myzone::ui::banner();
            identifyDevice(engine);
        } else if (choice == "2") {
            myzone::ui::clearConsole();
            myzone::ui::banner();
            searchDhcp(database);
        } else if (choice == "3") {
            myzone::ui::clearConsole();
            myzone::ui::banner();
            showSources(database);
        } else if (choice == "4") {
            myzone::ui::clearConsole();
            myzone::ui::banner();
            runDemo(engine);
        } else if (choice == "5") {
            myzone::ui::clearConsole();
            myzone::ui::banner();
            showHelp();
        } else if (choice == "0" || choice == "q" || choice == "Q") {
            break;
        } else {
            myzone::ui::warning("Option inconnue. Entrez un chiffre de 0 a 5.");
        }
    }

    std::cout << '\n';
    myzone::ui::drawFooter();
    return 0;
}
