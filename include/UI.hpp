#ifndef MYZONE_UI_HPP
#define MYZONE_UI_HPP

#include <string>

namespace myzone {
namespace ui {

// --- Codes de couleurs ANSI standards et vifs ---
constexpr const char* reset        = "\033[0m";
constexpr const char* bold         = "\033[1m";
constexpr const char* dim          = "\033[2m";

// Couleurs normales
constexpr const char* green        = "\033[32m";
constexpr const char* cyan         = "\033[36m";
constexpr const char* yellow       = "\033[33m";
constexpr const char* red          = "\033[31m";
constexpr const char* blue         = "\033[34m";
constexpr const char* magenta      = "\033[35m";
constexpr const char* white        = "\033[37m";

// Couleurs vives / intenses (style console de sécurité)
constexpr const char* greenBright  = "\033[1;32m";
constexpr const char* cyanBright   = "\033[1;36m";
constexpr const char* yellowBright = "\033[1;33m";
constexpr const char* redBright    = "\033[1;31m";
constexpr const char* whiteBright  = "\033[1;37m";

// --- Fonctions d'environnement & configuration console ---
void initConsole();
void clearConsole();
bool colorsEnabled();
void setQuiet(bool quiet);
bool isQuiet();

std::string color(const std::string& text, const char* colorCode);

// --- Balises de statut (style hacking / auditor) ---
std::string tagSuccess();  // [+] en vert vif
std::string tagInfo();     // [*] en cyan/bleu vif
std::string tagWarning();  // [!] en jaune/orange vif
std::string tagError();    // [-] en rouge vif
std::string tagPrompt();   // [?] en cyan vif

// --- Niveaux de journalisation ---
enum class LogLevel {
    Startup,
    Info,
    Pending,
    Success,
    Warning,
    Error,
    Complete
};

void log(LogLevel level, const std::string& text);

// --- Lignes de séparation (ASCII pur) ---
int getConsoleWidth();
void separator(char fillChar = '=', int length = 77);
void divider(char fillChar = '-', int length = 77);
void drawLine(const char* left, const char* fill, const char* right);
void drawFooter();

// --- Méthodes d'affichage typées ---
void banner();
void section(const std::string& text);
void title(const std::string& text);
void success(const std::string& text);
void warning(const std::string& text);
void error(const std::string& text);
void info(const std::string& text);
void item(const std::string& text);

// --- Entrées utilisateur ---
std::string prompt(const std::string& label);
void waitForEnter();

} // namespace ui
} // namespace myzone

#endif // MYZONE_UI_HPP
