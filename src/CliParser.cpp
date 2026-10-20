#include "../include/CliParser.hpp"
#include "../include/UI.hpp"

#include <iostream>

namespace myzone {

CliOptions CliParser::parse(int argc, char* argv[]) {
    CliOptions options;

    for (int i = 1; i < argc; ++i) {
        std::string arg = argv[i];
        if (arg == "--help" || arg == "-h") {
            options.help = true;
        } else if (arg == "--version" || arg == "-v") {
            options.version = true;
        } else if (arg == "--json") {
            options.json = true;
        } else if (arg == "--csv") {
            options.csv = true;
        } else if (arg == "--quiet" || arg == "-q") {
            options.quiet = true;
        } else if (arg == "--mac" && i + 1 < argc) {
            options.mac = argv[++i];
            options.hasIdArg = true;
        } else if (arg == "--hostname" && i + 1 < argc) {
            options.hostname = argv[++i];
            options.hasIdArg = true;
        } else if (arg == "--dhcp" && i + 1 < argc) {
            options.dhcpFp = argv[++i];
            options.hasIdArg = true;
        } else if (arg == "--tcp" && i + 1 < argc) {
            options.tcpSig = argv[++i];
            options.hasIdArg = true;
        } else if (arg == "--dhcp-options" && i + 1 < argc) {
            options.dhcpOpts = argv[++i];
            options.hasIdArg = true;
        }
    }

    return options;
}

void CliParser::printHelp() {
    ui::banner();
    std::cout << "Usage:\n"
              << "  MyZone                                      Mode interactif console\n"
              << "  MyZone --mac AA:BB:CC:DD:EE:FF [options]    Identification directe CLI\n\n"
              << "Options:\n"
              << "  --mac <address>        Adresse MAC de l'appareil (format libre : AA:BB...)\n"
              << "  --hostname <name>      Nom d'hote reseau (mDNS, NetBIOS, DHCP)\n"
              << "  --dhcp <hash>          Hash MD5 d'empreinte DHCP (32 caracteres)\n"
              << "  --tcp <signature>      Signature passive TCP (format p0f)\n"
              << "  --dhcp-options <list>  Options DHCP ordonnees (ex: 1,15,3,6,44)\n"
              << "  --json                 Sortie stricte au format JSON (scriptable)\n"
              << "  --csv                  Sortie stricte au format CSV (tableaux/audits)\n"
              << "  --quiet, -q            Mode silencieux (supprime bannières et logs)\n"
              << "  --help, -h             Afficher cette aide\n"
              << "  --version, -v          Afficher la version\n";
}

void CliParser::printVersion() {
    std::cout << "MyZone v1.1 - Network Device Fingerprinting Engine (C++17)\n";
}

} // namespace myzone
