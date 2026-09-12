#ifndef MYZONE_UI_HPP
#define MYZONE_UI_HPP

#include <string>

namespace myzone {
namespace ui {

/// @brief Code de couleur pour réinitialiser le formatage
constexpr const char* reset = "\033[0m";
/// @brief Code de formatage pour texte gras
constexpr const char* bold = "\033[1m";
/// @brief Code de formatage pour texte estompé
constexpr const char* dim = "\033[2m";
/// @brief Code de couleur Cyan
constexpr const char* cyan = "\033[36m";
/// @brief Code de couleur Bleu
constexpr const char* blue = "\033[34m";
/// @brief Code de couleur Vert
constexpr const char* green = "\033[32m";
/// @brief Code de couleur Jaune
constexpr const char* yellow = "\033[33m";
/// @brief Code de couleur Rouge
constexpr const char* red = "\033[31m";
/// @brief Code de couleur Magenta
constexpr const char* magenta = "\033[35m";

/// @brief Caractère pour le coin supérieur gauche d'une boîte
constexpr const char* boxTopLeft = "┌";
/// @brief Caractère pour le coin supérieur droit d'une boîte
constexpr const char* boxTopRight = "┐";
/// @brief Caractère pour le coin inférieur gauche d'une boîte
constexpr const char* boxBottomLeft = "└";
/// @brief Caractère pour le coin inférieur droit d'une boîte
constexpr const char* boxBottomRight = "┘";
/// @brief Caractère pour une ligne horizontale de boîte
constexpr const char* boxHorizontal = "─";
/// @brief Caractère pour une ligne verticale de boîte
constexpr const char* boxVertical = "│";
/// @brief Caractère pour une division gauche de boîte
constexpr const char* boxDividerLeft = "├";
/// @brief Caractère pour une division droite de boîte
constexpr const char* boxDividerRight = "┤";

/// @brief Initialise la console (support couleur, etc.)
void initConsole();
/// @brief Efface l'écran de la console
void clearConsole();

/// @brief Vérifie si les couleurs sont activées/supportées
/// @return true si les couleurs sont disponibles, false sinon
bool colorsEnabled();
/// @brief Colore un texte avec le code couleur fourni
/// @param text Le texte à colorer
/// @param colorCode Le code couleur ANSI (ex: cyan, red)
/// @return Le texte entouré des codes ANSI appropriés
std::string color(const std::string& text, const char* colorCode);

/// @brief Niveau de journalisation
enum class LogLevel {
    /// @brief Démarrage de l'application
    Startup,
    /// @brief Information standard
    Info,
    /// @brief Tâche en cours
    Pending,
    /// @brief Succès d'une opération
    Success,
    /// @brief Avertissement
    Warning,
    /// @brief Erreur
    Error,
    /// @brief Tâche terminée
    Complete
};

/// @brief Affiche un message dans les logs avec un certain niveau
/// @param level Le niveau de journalisation
/// @param text Le message à afficher
void log(LogLevel level, const std::string& text);

/// @brief Obtient la largeur actuelle de la console en caractères
/// @return La largeur de la console
int getConsoleWidth();
/// @brief Dessine une ligne décorative horizontale
/// @param left Le caractère/symbole à gauche
/// @param fill Le caractère de remplissage
/// @param right Le caractère/symbole à droite
void drawLine(const char* left, const char* fill, const char* right);
/// @brief Dessine un séparateur
void separator();
/// @brief Dessine le pied de page de l'interface
void drawFooter();
/// @brief Affiche un titre stylisé
/// @param text Le texte du titre
void title(const std::string& text);
/// @brief Affiche un message de succès stylisé
/// @param text Le message
void success(const std::string& text);
/// @brief Affiche un message d'avertissement stylisé
/// @param text Le message
void warning(const std::string& text);
/// @brief Affiche un message d'erreur stylisé
/// @param text Le message
void error(const std::string& text);
/// @brief Affiche une information stylisée
/// @param text Le message
void info(const std::string& text);
/// @brief Affiche un élément de liste ou une entrée
/// @param text Le texte de l'élément
void item(const std::string& text);

/// @brief Demande une saisie à l'utilisateur
/// @param label L'étiquette de la demande (prompt)
/// @return La chaîne saisie par l'utilisateur
std::string prompt(const std::string& label);
/// @brief Attend que l'utilisateur appuie sur Entrée
void waitForEnter();

} // namespace ui
} // namespace myzone

#endif // MYZONE_UI_HPP
