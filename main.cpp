#include <iostream>
#include <memory>

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
#include "include/DataLocator.hpp"
#include "include/CliParser.hpp"
#include "include/InteractiveCli.hpp"

int main(int argc, char* argv[]) {
    myzone::ui::initConsole();

    // 1. Analyse précoce des arguments de ligne de commande
    const myzone::CliOptions options = myzone::CliParser::parse(argc, argv);

    if (!options.error.empty()) {
        std::cerr << "MyZone: " << options.error << "\n"
                  << "Essayez 'MyZone --help' pour plus d'informations.\n";
        return myzone::CliParser::kUsageError;
    }

    if (options.help) {
        myzone::CliParser::printHelp();
        return 0;
    }
    if (options.version) {
        myzone::CliParser::printVersion();
        return 0;
    }

    // 2. Gestion du mode silencieux ou sorties brutes (JSON / CSV)
    if (options.json || options.csv || options.quiet) {
        myzone::ui::setQuiet(true);
    } else if (!options.hasIdArg) {
        myzone::ui::banner();
    }

    // 3. Localisation et chargement des référentiels
    const std::string dataDirectory = myzone::DataLocator::locate(argc > 0 ? argv[0] : "MyZone");
    if (!options.json && !options.csv && !options.quiet) {
        std::cout << myzone::ui::tagInfo() << " Chargement des bases de signatures ("
                  << myzone::ui::color(dataDirectory, myzone::ui::cyanBright) << ")...\n";
    }

    const myzone::Database database(dataDirectory);

    if (!database.isReady()) {
        myzone::ui::error("Aucune source OUI n'a ete chargee. Verifiez le dossier data/.");
        return 1;
    }

    // Configuration des modules d'identification
    myzone::IdentificationEngine engine;
    engine.addModule(std::make_unique<myzone::OuiModule>(database));
    engine.addModule(std::make_unique<myzone::DhcpModule>(database));
    engine.addModule(std::make_unique<myzone::HostnameModule>());
    engine.addModule(std::make_unique<myzone::P0fModule>(database));
    engine.addModule(std::make_unique<myzone::FingerBankModule>(database));

    if (!options.json && !options.csv && !options.quiet && !options.hasIdArg) {
        std::cout << myzone::ui::tagSuccess() << " " << database.vendorCount() << " prefixes OUI indexes\n";
        std::cout << myzone::ui::tagSuccess() << " " << database.dhcpFingerprintCount() << " empreintes DHCP indexees\n";
        std::cout << myzone::ui::tagSuccess() << " " << database.p0fSignatureCount() << " signatures TCP p0f indexees\n";
        std::cout << myzone::ui::tagSuccess() << " " << database.fingerBankCount() << " listes d'options FingerBank indexees\n";
        std::cout << myzone::ui::tagSuccess() << " " << engine.moduleCount() << " modules d'identification prets\n";
    }

    // 4. Exécution directe CLI avec arguments
    if (options.hasIdArg) {
        myzone::DeviceProfile input;
        if (!options.mac.empty()) {
            // Déjà validée par CliParser::parse
            input.mac = myzone::MacAddress(options.mac);
        }
        input.hostname = options.hostname;
        input.dhcpFingerprint = options.dhcpFp;
        input.tcpSignature = options.tcpSig;
        input.dhcpOptions = options.dhcpOpts;

        const myzone::DeviceProfile result = engine.identify(input);

        if (options.json) {
            std::cout << result.toJson() << "\n";
        } else if (options.csv) {
            std::cout << myzone::DeviceProfile::csvHeader() << "\n"
                      << result.toCsv() << "\n";
        } else {
            myzone::InteractiveCli::printProfile(result);
        }
        return 0;
    }

    // 5. Boucle interactive console
    myzone::InteractiveCli::run(database, engine);

    std::cout << '\n';
    myzone::ui::drawFooter();
    return 0;
}
