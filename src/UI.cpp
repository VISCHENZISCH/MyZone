#include "../include/UI.hpp"
#include <iostream>
#include <string>

#ifdef _WIN32
#include <windows.h>
#else
#include <sys/ioctl.h>
#include <unistd.h>
#endif

namespace myzone {
namespace ui {

void initConsole() {
#ifdef _WIN32
    SetConsoleOutputCP(CP_UTF8);
#endif
}

bool colorsEnabled() {
    return std::getenv("NO_COLOR") == nullptr;
}

std::string color(const std::string& text, const char* colorCode) {
    if (!colorsEnabled()) {
        return text;
    }
    return std::string(colorCode) + text + reset;
}

int getConsoleWidth() {
    int width = 80;
#ifdef _WIN32
    CONSOLE_SCREEN_BUFFER_INFO csbi;
    if (GetConsoleScreenBufferInfo(GetStdHandle(STD_OUTPUT_HANDLE), &csbi)) {
        width = csbi.srWindow.Right - csbi.srWindow.Left + 1;
    }
#else
    struct winsize w;
    if (ioctl(STDOUT_FILENO, TIOCGWINSZ, &w) != -1) {
        width = w.ws_col;
    }
#endif
    return width < 40 ? 80 : width;
}

void drawLine(const char* left, const char* fill, const char* right) {
    int width = getConsoleWidth();
    std::cout << color(left, blue);
    for (int i = 0; i < width - 2; ++i) {
        std::cout << color(fill, blue);
    }
    std::cout << color(right, blue) << '\n';
}

void drawFooter() {
    drawLine(boxBottomLeft, boxHorizontal, boxBottomRight);
}

void title(const std::string& text) {
    std::cout << '\n' << color("  " + text, cyan) << '\n';
}

void success(const std::string& text) {
    std::cout << "  " << color("OK", green) << " " << text << '\n';
}

void warning(const std::string& text) {
    std::cout << "  " << color("!", yellow) << " " << text << '\n';
}

void error(const std::string& text) {
    std::cout << "  " << color("X", red) << " " << text << '\n';
}

std::string prompt(const std::string& label) {
    std::cout << '\n' << color("  > ", magenta) << label << std::flush;
    std::string value;
    std::getline(std::cin, value);
    return value;
}

void waitForEnter() {
    std::cout << '\n' << color("  Appuyez sur Entrée pour continuer...", dim) << std::flush;
    std::string ignored;
    std::getline(std::cin, ignored);
}

void printMenu() {
    std::cout << '\n';
    drawLine(boxTopLeft, boxHorizontal, boxTopRight);
    std::cout << color(boxVertical, blue) << color("  TABLEAU DE BORD", bold)
              << std::string(getConsoleWidth() - 2 - 17, ' ') << color(boxVertical, blue) << '\n';
    drawLine(boxDividerLeft, boxHorizontal, boxDividerRight);

    auto printOption = [](const char* key, const char* desc) {
        std::string keyStr(key);
        std::string descStr(desc);
        int width = getConsoleWidth();
        int padLen = width - 2 - 4 - keyStr.length() - descStr.length();
        if (padLen < 0) padLen = 0;
        std::string padding(padLen, ' ');
        std::cout << color(boxVertical, blue)
                  << "  " << color(keyStr, cyan) << "  " << descStr
                  << padding << color(boxVertical, blue) << '\n';
    };

    printOption("1", "Rechercher un appareil par adresse MAC");
    printOption("0", "Quitter");

    drawLine(boxBottomLeft, boxHorizontal, boxBottomRight);
}

} // namespace ui
} // namespace myzone
