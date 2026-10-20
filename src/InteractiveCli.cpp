#include "../include/InteractiveCli.hpp"
#include "../include/UI.hpp"

#include <iostream>
#include <iomanip>
#include <string>
#include <vector>

namespace myzone {

namespace {

std::string padRight(const std::string& str, std::size_t width) {
    if (str.size() >= width) return str;
    return str + std::string(width - str.size(), ' ');
}

std::string padLeft(const std::string& str, std::size_t width) {
    if (str.size() >= width) return str;
    return std::string(width - str.size(), ' ') + str;
}

void printMainMenu() {
    std::cout << '\n';
    std::cout << ui::tagSuccess() << " " << ui::color("Options disponibles :", ui::whiteBright) << "\n\n";
    std::cout << "  " << ui::color("[1]", ui::greenBright) << " Identifier un appareil (MAC, DHCP, hostname, TCP)\n";
    std::cout << "  " << ui::color("[2]", ui::greenBright) << " Rechercher une empreinte DHCP connue\n";
    std::cout << "  " << ui::color("[3]", ui::greenBright) << " Voir le catalogue des sources et signatures\n";
    std::cout << "  " << ui::color("[4]", ui::greenBright) << " Lancer une demonstration avec des cibles d'exemple\n";
    std::cout << "  " << ui::color("[5]", ui::greenBright) << " Afficher l'aide et les limites d'identification\n";
    std::cout << "  " << ui::color("[0]", ui::redBright)   << " Quitter\n\n";
}

} // namespace

void InteractiveCli::printProfile(const DeviceProfile& profile) {
    std::cout << '\n';
    std::cout << ui::tagSuccess() << " " << ui::color("Resultat d'identification :", ui::whiteBright) << '\n';
    ui::divider('-');

    if (profile.mac.isValid()) {
        std::cout << "    " << ui::color("Adresse MAC   : ", ui::dim)
                  << ui::color(profile.mac.toString(), ui::greenBright);

        if (profile.mac.isLocallyAdministered()) {
            std::cout << " " << ui::color("[MAC Privée / Aléatoire - bit LAA]", ui::yellowBright);
        } else if (profile.mac.isMulticast()) {
            std::cout << " " << ui::color("[Multicast]", ui::cyanBright);
        }
        std::cout << '\n';
    }

    if (!profile.manufacturer.empty()) {
        std::cout << "    " << ui::color("Fabricant     : ", ui::dim)
                  << ui::color(profile.manufacturer, ui::whiteBright) << '\n';
    }
    if (profile.category != DeviceCategory::Unknown) {
        std::cout << "    " << ui::color("Categorie     : ", ui::dim)
                  << ui::color(toString(profile.category), ui::cyanBright) << '\n';
    }
    if (!profile.osName.empty()) {
        std::cout << "    " << ui::color("OS / Appareil : ", ui::dim)
                  << ui::color(profile.osName, ui::yellowBright) << '\n';
    }
    if (!profile.deviceModel.empty() && profile.deviceModel != profile.osName) {
        std::cout << "    " << ui::color("Modele        : ", ui::dim)
                  << ui::color(profile.deviceModel, ui::cyanBright) << '\n';
    }

    const int confidence = profile.calculateConfidence();
    if (confidence > 0) {
        const char* confColor = confidence >= 80 ? ui::greenBright : (confidence >= 50 ? ui::yellowBright : ui::redBright);
        std::cout << "    " << ui::color("Indice fiable : ", ui::dim)
                  << ui::color(std::to_string(confidence) + " % (" + profile.confidenceLevel() + ")", confColor) << '\n';
    }

    if (!profile.results.empty()) {
        std::cout << '\n';
        std::cout << "    " << ui::color("Modules actifs :", ui::whiteBright) << '\n';
        for (const auto& result : profile.results) {
            std::cout << "    " << ui::tagInfo() << " ["
                      << ui::color(result.moduleName, ui::greenBright) << "] "
                      << result.detail << '\n';
        }
    }
    ui::divider('-');
}

void InteractiveCli::identifyDevice(const IdentificationEngine& engine) {
    std::cout << '\n';
    std::cout << ui::tagSuccess() << " " << ui::color("Mode Identification d'appareil", ui::whiteBright) << '\n';
    ui::divider('-');
    std::cout << "  " << ui::tagInfo() << " Renseignez les elements connus (appuyez sur Entree pour ignorer)\n";
    std::cout << "  " << ui::tagInfo() << " Formats MAC : AA:BB:CC:DD:EE:FF | AA-BB-CC-DD-EE-FF | AABBCCDDEEFF\n\n";

    const std::string rawMac   = ui::prompt("Adresse MAC              : ");
    const std::string dhcpFp   = ui::prompt("Empreinte DHCP (hash MD5): ");
    const std::string hostname = ui::prompt("Nom d'hote (Hostname)    : ");
    const std::string tcpSig   = ui::prompt("Signature TCP (p0f)      : ");
    const std::string dhcpOpts = ui::prompt("Options DHCP (ex: 1,3,6) : ");

    if (rawMac.empty() && dhcpFp.empty() && hostname.empty() && tcpSig.empty() && dhcpOpts.empty()) {
        std::cout << '\n';
        ui::warning("Aucune donnee fournie pour l'identification.");
        ui::waitForEnter();
        return;
    }

    DeviceProfile input;

    if (!rawMac.empty()) {
        try {
            input.mac = MacAddress(rawMac);
        } catch (const std::invalid_argument& e) {
            std::cout << '\n';
            ui::error(e.what());
            ui::waitForEnter();
            return;
        }
    }

    input.dhcpFingerprint = dhcpFp;
    input.hostname = hostname;
    input.tcpSignature = tcpSig;
    input.dhcpOptions = dhcpOpts;

    const DeviceProfile result = engine.identify(input);

    if (!result.hasResults()) {
        std::cout << '\n';
        ui::warning("Aucun module n'a pu identifier cet appareil avec les elements fournis.");
    } else {
        printProfile(result);
    }

    ui::waitForEnter();
}

void InteractiveCli::searchDhcp(const Database& database) {
    std::cout << '\n';
    std::cout << ui::tagSuccess() << " " << ui::color("Recherche d'empreinte DHCP (KYD / FingerBank)", ui::whiteBright) << '\n';
    ui::divider('-');
    std::cout << "  " << ui::tagInfo() << " Entrez un hash MD5 d'empreinte DHCP (32 caracteres hexadecimaux)\n\n";

    const std::string hash = ui::prompt("Hash MD5 : ");
    if (hash.empty()) {
        std::cout << '\n';
        ui::warning("Aucune empreinte fournie.");
        ui::waitForEnter();
        return;
    }

    DhcpFingerprintInfo info;
    std::cout << '\n';
    if (database.lookupDhcpFingerprint(hash, info)) {
        std::cout << ui::tagSuccess() << " " << ui::color("Correspondance DHCP identifiee :", ui::whiteBright) << '\n';
        ui::divider('-');
        std::cout << "    " << ui::color("Empreinte : ", ui::dim)
                  << ui::color(info.fingerprint, ui::cyanBright) << '\n';
        std::cout << "    " << ui::color("Appareil  : ", ui::dim)
                  << ui::color(info.deviceName, ui::whiteBright) << '\n';
        if (!info.requestedOptions.empty()) {
            std::cout << "    " << ui::color("Options   : ", ui::dim)
                      << info.requestedOptions << '\n';
        }
        std::cout << "    " << ui::color("Confiance : ", ui::dim)
                  << ui::color(std::to_string(info.confidence) + " %", ui::greenBright) << '\n';
        ui::divider('-');
    } else {
        ui::warning("Aucune correspondance trouvee pour ce hash MD5.");
    }

    ui::waitForEnter();
}

void InteractiveCli::showSources(const Database& database) {
    std::cout << '\n';
    std::cout << ui::tagSuccess() << " " << ui::color("Catalogue des sources & bases locales", ui::whiteBright) << '\n';
    ui::divider('-');
    std::cout << "  " << ui::tagInfo() << " Repertoire des donnees : "
              << ui::color(database.dataDirectory(), ui::cyanBright) << '\n';
    std::cout << "  " << ui::tagInfo() << " Total indexes : "
              << ui::color(std::to_string(database.vendorCount()), ui::greenBright) << " OUI  |  "
              << ui::color(std::to_string(database.dhcpFingerprintCount()), ui::greenBright) << " DHCP  |  "
              << ui::color(std::to_string(database.p0fSignatureCount()), ui::greenBright) << " p0f TCP  |  "
              << ui::color(std::to_string(database.fingerBankCount()), ui::greenBright) << " FingerBank\n\n";

    std::cout << "  "
              << ui::color("FICHIER", ui::greenBright)
              << std::string(24 - 7, ' ')
              << ui::color("ENTREES", ui::greenBright)
              << "   "
              << ui::color("STATUT", ui::greenBright)
              << std::string(12 - 6, ' ')
              << ui::color("ROLE / DESCRIPTION", ui::greenBright)
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
            ? (source.indexed ? ui::greenBright : ui::yellowBright)
            : ui::redBright;

        std::string recordsStr = source.records > 0 ? std::to_string(source.records) : "-";

        std::cout << "  "
                  << padRight(source.fileName, 22) << "  "
                  << padLeft(recordsStr, 8) << "  "
                  << ui::color(padRight(statusText, 10), statusColor) << "  "
                  << source.purpose << '\n';
    }

    std::cout << '\n';
    ui::waitForEnter();
}

void InteractiveCli::runDemo(const IdentificationEngine& engine) {
    std::cout << '\n';
    std::cout << ui::tagSuccess() << " " << ui::color("Demonstration sur des cibles d'exemple", ui::whiteBright) << '\n';
    ui::divider('-');
    std::cout << "  " << ui::tagInfo() << " Evaluation automatique de configurations d'appareils\n\n";

    struct DemoEntry {
        std::string label;
        std::string mac;
        std::string hostname;
        std::string dhcpOptions;
    };

    const std::vector<DemoEntry> demos = {
        {"Apple iPhone (MAC officielle)",     "AC:DE:48:00:11:22", "",                   ""},
        {"Samsung Galaxy (via hostname)",      "",                  "Galaxy-S23-Ultra",   ""},
        {"Cisco routeur (via MAC)",            "00:1A:2B:00:00:00", "",                   ""},
        {"Windows PC (via hostname)",          "",                  "DESKTOP-AB1CD2E",    ""},
        {"Google Pixel (via hostname)",        "",                  "Pixel-8-Pro",        ""},
        {"MacBook (via hostname)",             "",                  "MacBook-Pro-de-Tom", ""},
        {"iPad (via hostname)",                "",                  "iPad-de-Marie",      ""},
        {"Raspberry Pi (via MAC)",             "B8:27:EB:12:34:56", "",                   ""},
        {"TP-Link appareil (via MAC)",         "50:C7:BF:AA:BB:CC", "",                   ""},
        {"Appareil MAC Randomisée",            "DA:A1:19:12:34:56", "",                   ""},
    };

    std::cout << "  "
              << ui::color("NUM", ui::greenBright) << "   "
              << ui::color("CIBLE", ui::greenBright) << std::string(32 - 5, ' ')
              << ui::color("ENTREE", ui::greenBright) << std::string(22 - 6, ' ')
              << ui::color("RESULTAT / IDENTIFICATION", ui::greenBright) << '\n';

    std::cout << "  "
              << std::string(4, '-')  << "  "
              << std::string(30, '-') << "  "
              << std::string(20, '-') << "  "
              << std::string(36, '-') << '\n';

    for (std::size_t i = 0; i < demos.size(); ++i) {
        const auto& demo = demos[i];
        DeviceProfile input;

        std::string testValue;
        if (!demo.mac.empty()) {
            try { input.mac = MacAddress(demo.mac); testValue = demo.mac; } catch (...) {}
        }
        if (!demo.hostname.empty()) {
            input.hostname = demo.hostname;
            if (testValue.empty()) testValue = demo.hostname;
        }

        const DeviceProfile result = engine.identify(input);

        std::string identification;
        if (!result.manufacturer.empty()) {
            identification = result.manufacturer;
        }
        if (result.category != DeviceCategory::Unknown) {
            if (!identification.empty()) identification += " [";
            identification += toString(result.category);
            if (!result.manufacturer.empty()) identification += "]";
        } else if (!result.osName.empty()) {
            if (!identification.empty()) identification += " [";
            identification += result.osName;
            if (!result.manufacturer.empty()) identification += "]";
        }

        if (input.mac.isValid() && input.mac.isLocallyAdministered()) {
            identification += " " + ui::color("[MAC Random]", ui::yellowBright);
        }

        if (identification.empty()) {
            identification = ui::color("Inconnu", ui::dim);
        }

        std::string numStr = "[" + std::to_string(i + 1) + "]";
        std::cout << "  "
                  << ui::color(padRight(numStr, 4), ui::greenBright) << "  "
                  << padRight(demo.label, 30) << "  "
                  << ui::color(padRight(testValue, 20), ui::cyanBright) << "  "
                  << identification << '\n';
    }

    std::cout << '\n';
    ui::waitForEnter();
}

void InteractiveCli::showHelp() {
    std::cout << '\n';
    std::cout << ui::tagSuccess() << " " << ui::color("Aide & Limites d'identification MyZone", ui::whiteBright) << '\n';
    ui::divider('-');

    std::cout << '\n' << "  " << ui::color("MODULES D'IDENTIFICATION :", ui::greenBright) << '\n';
    std::cout << "    " << ui::tagSuccess() << " " << ui::color("OUI", ui::whiteBright)
              << "         Identifie le constructeur via le prefixe MAC (IEEE / Wireshark / Nmap)\n";
    std::cout << "    " << ui::tagSuccess() << " " << ui::color("DHCP", ui::whiteBright)
              << "        Recherche le modele et l'OS par hash MD5 d'options DHCP (KYD / FingerBank)\n";
    std::cout << "    " << ui::tagSuccess() << " " << ui::color("Hostname", ui::whiteBright)
              << "    Analyse heuristique du nom d'hote reseau (ex: Apple, Samsung, Windows)\n";
    std::cout << "    " << ui::tagSuccess() << " " << ui::color("p0f TCP", ui::whiteBright)
              << "     Identification passive de l'OS par signature SYN TCP/IP (p0f v3)\n";
    std::cout << "    " << ui::tagSuccess() << " " << ui::color("FingerBank", ui::whiteBright)
              << "  Identification de l'OS via la liste ordonnee d'options DHCP demandees\n";

    std::cout << '\n' << "  " << ui::color("FORMATS ACCEPTES :", ui::greenBright) << '\n';
    std::cout << "    " << ui::tagInfo() << " Adresse MAC   : AA:BB:CC:DD:EE:FF | AA-BB-CC-DD-EE-FF | AABBCCDDEEFF\n";
    std::cout << "    " << ui::tagInfo() << " Hash DHCP     : Chaine hexadecimale de 32 caracteres (MD5)\n";
    std::cout << "    " << ui::tagInfo() << " Hostname      : Nom machine NetBIOS / mDNS (ex: DESKTOP-AB1CD2E, iPhone-de-Tom)\n";
    std::cout << "    " << ui::tagInfo() << " Signature TCP : Signature au format p0f (ex: *:64:0:*:mss*20,7:mss,sok,...)\n";
    std::cout << "    " << ui::tagInfo() << " Options DHCP  : Liste d'options separees par virgules (ex: 1,15,3,6,44)\n";

    std::cout << '\n' << "  " << ui::color("LIMITES & CONDITIONS D'AUDIT :", ui::yellowBright) << '\n';
    std::cout << "    " << ui::tagWarning() << " L'adresse MAC identifie un bloc attribue a un constructeur, pas un modele unique.\n";
    std::cout << "    " << ui::tagWarning() << " Les adresses MAC randomisees (Wi-Fi prive iOS/Android) masquent le fabricant officiel.\n";
    std::cout << "    " << ui::tagWarning() << " Le nom d'hote (hostname) peut etre modifie arbitrairement par l'utilisateur.\n";
    std::cout << "    " << ui::tagWarning() << " Les modules fonctionnent par heuristique et correlations passives.\n";
    std::cout << "    " << ui::tagWarning() << " MyZone est 100% passif et local : aucune trame n'est envoyee sur le reseau.\n\n";

    ui::waitForEnter();
}

void InteractiveCli::run(const Database& database, const IdentificationEngine& engine) {
    bool firstMenu = true;
    while (true) {
        if (!firstMenu) {
            ui::clearConsole();
            ui::banner();
        }
        firstMenu = false;

        printMainMenu();
        const std::string choice = ui::prompt("Choix : ");
        if (!std::cin) {
            std::cout << '\n';
            break;
        }

        if (choice == "1") {
            ui::clearConsole();
            ui::banner();
            identifyDevice(engine);
        } else if (choice == "2") {
            ui::clearConsole();
            ui::banner();
            searchDhcp(database);
        } else if (choice == "3") {
            ui::clearConsole();
            ui::banner();
            showSources(database);
        } else if (choice == "4") {
            ui::clearConsole();
            ui::banner();
            runDemo(engine);
        } else if (choice == "5") {
            ui::clearConsole();
            ui::banner();
            showHelp();
        } else if (choice == "0" || choice == "q" || choice == "Q") {
            break;
        } else {
            ui::warning("Option inconnue. Entrez un chiffre de 0 a 5.");
        }
    }
}

} // namespace myzone
