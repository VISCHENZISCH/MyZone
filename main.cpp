#include <cstdlib>
#include <filesystem>
#include <iostream>
#include <stdexcept>
#include <string>
#include <vector>

#include "include/Database.hpp"
#include "include/DeviceCategory.hpp"
#include "include/MacAddress.hpp"

namespace fs = std::filesystem;

namespace {

namespace ui {

constexpr const char* reset = "\033[0m";
constexpr const char* bold = "\033[1m";
constexpr const char* dim = "\033[2m";
constexpr const char* cyan = "\033[36m";
constexpr const char* blue = "\033[34m";
constexpr const char* green = "\033[32m";
constexpr const char* yellow = "\033[33m";
constexpr const char* red = "\033[31m";

bool colorsEnabled() {
    return std::getenv("NO_COLOR") == nullptr;
}

std::string color(const std::string& text, const char* colorCode) {
    if (!colorsEnabled()) {
        return text;
    }
    return std::string(colorCode) + text + reset;
}

void rule() {
    std::cout << color("----------------------------------------------------------------", blue) << '\n';
}

void title(const std::string& text) {
    rule();
    std::cout << color("  " + text, bold) << '\n';
    rule();
}

void success(const std::string& text) {
    std::cout << color("  OK  ", green) << text << '\n';
}

void warning(const std::string& text) {
    std::cout << color("  !   ", yellow) << text << '\n';
}

void error(const std::string& text) {
    std::cout << color("  X   ", red) << text << '\n';
}

std::string prompt(const std::string& label) {
    std::cout << color("  > " + label, cyan) << std::flush;
    std::string value;
    std::getline(std::cin, value);
    return value;
}

void waitForEnter() {
    std::cout << color("\n  Appuyez sur Entrée pour continuer...", dim) << std::flush;
    std::string ignored;
    std::getline(std::cin, ignored);
}

} // namespace ui

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

std::string formatCount(std::size_t value) {
    std::string digits = std::to_string(value);
    for (std::ptrdiff_t position = static_cast<std::ptrdiff_t>(digits.size()) - 3;
         position > 0; position -= 3) {
        digits.insert(static_cast<std::size_t>(position), " ");
    }
    return digits;
}

void printWelcome(const myzone::Database& database) {
    std::cout << '\n';
    ui::title("MYZONE  /  NETWORK INTELLIGENCE");
    std::cout << "  Identification locale des appareils par MAC et empreinte DHCP\n\n";
    ui::success(formatCount(database.vendorCount()) + " fabricants/OUI uniques prêts pour la recherche");
    ui::success(formatCount(database.dhcpFingerprintCount()) + " empreintes DHCP indexées");
    std::cout << ui::color("  Données : " + database.dataDirectory(), ui::dim) << "\n\n";
}

void printDevice(const myzone::MacAddress& mac, const myzone::DeviceInfo& info) {
    ui::title("RÉSULTAT  /  ADRESSE MAC");
    std::cout << "  Adresse      " << ui::color(mac.toString(), ui::cyan) << '\n';
    std::cout << "  Fabricant    " << ui::color(info.companyName, ui::bold) << '\n';
    std::cout << "  Catégorie    " << myzone::toString(info.category) << '\n';
    std::cout << "  Source       " << info.source << "\n";
}

void lookupMac(const myzone::Database& database) {
    ui::title("RECHERCHE  /  ADRESSE MAC");
    std::cout << "  Formats acceptés : AA:BB:CC:DD:EE:FF, AA-BB-CC-DD-EE-FF ou AABBCCDDEEFF\n\n";
    const std::string rawMac = ui::prompt("Adresse MAC (Entrée pour retour) : ");
    if (rawMac.empty()) {
        return;
    }

    try {
        const myzone::MacAddress mac(rawMac);
        myzone::DeviceInfo info;
        if (database.lookup(mac, info)) {
            printDevice(mac, info);
        } else {
            ui::warning("Adresse valide, mais aucun fabricant n'a été trouvé dans les sources OUI chargées.");
        }
    } catch (const std::invalid_argument& exception) {
        ui::error(exception.what());
    }
    ui::waitForEnter();
}

void lookupDhcp(const myzone::Database& database) {
    ui::title("RECHERCHE  /  EMPREINTE DHCP");
    std::cout << "  Saisissez le hash MD5 de l'empreinte DHCP (32 caractères hexadécimaux).\n";
    std::cout << "  Exemple : ac8b90de1120d9e3e2a68354458de76a\n\n";
    const std::string hash = ui::prompt("Empreinte DHCP (Entrée pour retour) : ");
    if (hash.empty()) {
        return;
    }

    myzone::DhcpFingerprintInfo info;
    if (!database.lookupDhcpFingerprint(hash, info)) {
        ui::warning("Empreinte absente de la base KYD / FingerBank.");
        ui::waitForEnter();
        return;
    }

    ui::title("RÉSULTAT  /  EMPREINTE DHCP");
    std::cout << "  Appareil      " << ui::color(info.deviceName, ui::bold) << '\n';
    std::cout << "  Confiance     " << ui::color(std::to_string(info.confidence) + " / 100", ui::green) << '\n';
    std::cout << "  Options DHCP  " << info.requestedOptions << '\n';
    std::cout << "  Source        KYD / FingerBank DHCP\n";
    ui::waitForEnter();
}

void showSources(const myzone::Database& database) {
    ui::title("CATALOGUE  /  DONNÉES LOCALES");
    for (const myzone::DataSourceInfo& source : database.sources()) {
        const std::string state = !source.available ? "ABSENT"
            : (source.indexed ? "INDEXÉ" : "DISPONIBLE");
        const char* stateColor = !source.available ? ui::red
            : (source.indexed ? ui::green : ui::yellow);
        std::cout << "  [" << ui::color(state, stateColor) << "] "
                  << ui::color(source.label, ui::bold) << '\n';
        std::cout << "      " << source.fileName << " — " << source.purpose;
        if (source.available && source.records > 0) {
            std::cout << " (" << formatCount(source.records)
                      << (source.indexed ? " enregistrements lus)" : " lignes utiles)");
        }
        std::cout << '\n';
    }
    std::cout << "\n" << ui::color("  Les signatures Nmap, p0f et FingerBank sont disponibles localement ", ui::dim) << '\n';
    std::cout << ui::color("  pour le futur module de scan/fingerprinting ; aucune analyse réseau n'est lancée ici.", ui::dim) << '\n';
    ui::waitForEnter();
}

void runDemo(const myzone::Database& database) {
    ui::title("DÉMONSTRATION  /  RECHERCHE OUI");
    const std::vector<std::string> samples = {
        "C4:A0:52:11:22:33",
        "3C:08:CD:AA:BB:CC",
        "00:1A:EB:99:88:77",
        "11:22:33:44:55:66"
    };

    for (const std::string& sample : samples) {
        const myzone::MacAddress mac(sample);
        myzone::DeviceInfo info;
        std::cout << "\n  " << ui::color(mac.toString(), ui::cyan) << "  ";
        if (database.lookup(mac, info)) {
            std::cout << ui::color(info.companyName, ui::bold)
                      << " - " << myzone::toString(info.category) << '\n';
        } else {
            std::cout << "fabricant inconnu\n";
        }
    }
    ui::waitForEnter();
}

void showHelp() {
    ui::title("         MYZONE      ");
    std::cout << "  MyZone associe le préfixe MAC (OUI) à un fabricant, puis affiche la\n";
    std::cout << "  catégorie disponible dans le référentiel enrichi. Les bases Wireshark\n";
    std::cout << "  et Nmap complètent la recherche si le référentiel principal ne répond pas.\n\n";
    std::cout << "  L'identification DHCP utilise la base KYD / FingerBank à partir d'un hash\n";
    std::cout << "  déjà calculé. Les résultats restent indicatifs : une MAC identifie le\n";
    std::cout << "  fabricant du matériel, pas nécessairement l'appareil exact.\n";
    ui::waitForEnter();
}

void printMenu() {
    ui::title("TABLEAU DE BORD");
    std::cout << "  " << ui::color("1", ui::cyan) << "  Rechercher un appareil par adresse MAC\n";
    std::cout << "  " << ui::color("2", ui::cyan) << "  Rechercher une empreinte DHCP\n";
    std::cout << "  " << ui::color("3", ui::cyan) << "  Consulter les sources de données\n";
    std::cout << "  " << ui::color("4", ui::cyan) << "  Lancer la démonstration\n";
    std::cout << "  " << ui::color("5", ui::cyan) << "  Aide\n";
    std::cout << "  " << ui::color("0", ui::cyan) << "  Quitter\n\n";
}

} // namespace

int main(int argc, char* argv[]) {
    const std::string dataDirectory = locateDataDirectory(argc > 0 ? argv[0] : "MyZone");
    const myzone::Database database(dataDirectory);

    printWelcome(database);
    if (!database.isReady()) {
        ui::error("Aucune source OUI n'a été chargée. Vérifiez le dossier data/.");
        return 1;
    }

    for (;;) {
        printMenu();
        const std::string choice = ui::prompt("Votre choix : ");
        if (!std::cin) {
            std::cout << '\n';
            break;
        }

        if (choice == "1") {
            lookupMac(database);
        } else if (choice == "2") {
            lookupDhcp(database);
        } else if (choice == "3") {
            showSources(database);
        } else if (choice == "4") {
            runDemo(database);
        } else if (choice == "5") {
            showHelp();
        } else if (choice == "0" || choice == "q" || choice == "Q") {
            break;
        } else {
            ui::warning("Choix inconnu. Sélectionnez une option du tableau de bord.");
        }
    }

    std::cout << "\n" << ui::color("  MyZone arrêté. À bientôt sur votre réseau.", ui::green) << "\n\n";
    return 0;
}
