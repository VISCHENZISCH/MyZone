#include "../include/CliParser.hpp"
#include "../include/UI.hpp"
#include "../include/MacAddress.hpp"
#include "Version.hpp"

#include <iostream>

namespace myzone {

CliOptions CliParser::parse(int argc, char* argv[]) {
    CliOptions options;

    // Associe chaque option à valeur au champ qu'elle renseigne
    const auto valueTarget = [&options](const std::string& arg) -> std::string* {
        if (arg == "--mac") return &options.mac;
        if (arg == "--hostname") return &options.hostname;
        if (arg == "--dhcp") return &options.dhcpFp;
        if (arg == "--tcp") return &options.tcpSig;
        if (arg == "--dhcp-options") return &options.dhcpOpts;
        return nullptr;
    };

    for (int i = 1; i < argc; ++i) {
        const std::string arg = argv[i];
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
        } else if (std::string* target = valueTarget(arg)) {
            if (i + 1 >= argc) {
                options.error = "l'option " + arg + " attend une valeur";
                return options;
            }
            *target = argv[++i];
            if (target->empty()) {
                options.error = "l'option " + arg + " attend une valeur non vide";
                return options;
            }
            options.hasIdArg = true;
        } else {
            options.error = "argument inconnu : " + arg;
            return options;
        }
    }

    if (options.help || options.version) {
        return options;
    }
    if (options.json && options.csv) {
        options.error = "--json et --csv sont mutuellement exclusifs";
    } else if ((options.json || options.csv) && !options.hasIdArg) {
        options.error = "--json et --csv nécessitent au moins un identifiant (--mac, --hostname, --dhcp, --tcp, --dhcp-options)";
    } else if (!options.mac.empty() && !MacAddress::tryParse(options.mac)) {
        options.error = "adresse MAC invalide : " + options.mac;
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
              << "  --version, -v          Afficher la version\n\n"
              << "Codes de sortie : 0 succes, 1 bases introuvables, 2 erreur d'utilisation\n";
}

void CliParser::printVersion() {
    std::cout << "MyZone v" << kVersion << " - Network Device Fingerprinting Engine (C++17)\n";
}

} // namespace myzone
