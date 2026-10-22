#ifndef MYZONE_CLI_PARSER_HPP
#define MYZONE_CLI_PARSER_HPP

#include <string>

namespace myzone {

/// @brief Arguments passés en ligne de commande
struct CliOptions {
    std::string mac;
    std::string hostname;
    std::string dhcpFp;
    std::string tcpSig;
    std::string dhcpOpts;
    bool json = false;
    bool csv = false;
    bool quiet = false;
    bool help = false;
    bool version = false;
    bool hasIdArg = false;
    /// @brief Message d'erreur d'analyse ; vide si les arguments sont valides
    std::string error;
};

/// @brief Analyseur d'arguments CLI
class CliParser {
public:
    /// @brief Analyse les arguments passés au binaire
    /// @return Options analysées ; CliOptions::error est renseigné en cas d'argument invalide
    static CliOptions parse(int argc, char* argv[]);

    /// @brief Affiche l'aide en ligne de commande
    static void printHelp();

    /// @brief Affiche la version
    static void printVersion();

    /// @brief Code de sortie en cas d'erreur d'utilisation (convention sysexits/getopt)
    static constexpr int kUsageError = 2;
};

} // namespace myzone

#endif // MYZONE_CLI_PARSER_HPP
