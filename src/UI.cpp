#include "../include/UI.hpp"
#include <cstdlib>
#include <iostream>
#include <iomanip>
#include <string>

#ifdef _WIN32
#include <windows.h>
#else
#include <sys/ioctl.h>
#include <unistd.h>
#endif

namespace myzone {
namespace ui {

namespace {
bool g_quiet = false;
}

void initConsole() {
#ifdef _WIN32
    SetConsoleOutputCP(CP_UTF8);
#endif
}

void clearConsole() {
    if (g_quiet) return;
#ifdef _WIN32
    HANDLE hStdOut = GetStdHandle(STD_OUTPUT_HANDLE);
    if (hStdOut == INVALID_HANDLE_VALUE) return;
    CONSOLE_SCREEN_BUFFER_INFO csbi;
    if (!GetConsoleScreenBufferInfo(hStdOut, &csbi)) return;
    DWORD cellCount = csbi.dwSize.X * csbi.dwSize.Y;
    DWORD count;
    COORD homeCoords = { 0, 0 };
    FillConsoleOutputCharacter(hStdOut, ' ', cellCount, homeCoords, &count);
    FillConsoleOutputAttribute(hStdOut, csbi.wAttributes, cellCount, homeCoords, &count);
    SetConsoleCursorPosition(hStdOut, homeCoords);
#else
    std::cout << "\033[2J\033[H" << std::flush;
#endif
}

bool colorsEnabled() {
    return std::getenv("NO_COLOR") == nullptr;
}

void setQuiet(bool quiet) {
    g_quiet = quiet;
}

bool isQuiet() {
    return g_quiet;
}

std::string color(const std::string& text, const char* colorCode) {
    if (!colorsEnabled() || text.empty()) {
        return text;
    }
    return std::string(colorCode) + text + reset;
}

std::string tagSuccess() {
    return color("[+]", greenBright);
}

std::string tagInfo() {
    return color("[*]", cyanBright);
}

std::string tagWarning() {
    return color("[!]", yellowBright);
}

std::string tagError() {
    return color("[-]", redBright);
}

std::string tagPrompt() {
    return color("[?]", cyanBright);
}

void log(const LogLevel level, const std::string& text) {
    if (g_quiet) return;
    std::string tag;
    switch (level) {
    case LogLevel::Startup:
    case LogLevel::Info:
    case LogLevel::Pending:
        tag = tagInfo();
        break;
    case LogLevel::Success:
    case LogLevel::Complete:
        tag = tagSuccess();
        break;
    case LogLevel::Warning:
        tag = tagWarning();
        break;
    case LogLevel::Error:
        tag = tagError();
        break;
    }
    std::cout << tag << " " << text << '\n';
}

int getConsoleWidth() {
    int width = 77;
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
    return width < 40 ? 77 : (width > 90 ? 77 : width);
}

void separator(char fillChar, int length) {
    if (g_quiet) return;
    int len = length > 0 ? length : getConsoleWidth();
    std::cout << color(std::string(len, fillChar), dim) << '\n';
}

void divider(char fillChar, int length) {
    if (g_quiet) return;
    int len = length > 0 ? length : getConsoleWidth();
    std::cout << color(std::string(len, fillChar), dim) << '\n';
}

void drawLine(const char* left, const char* fill, const char* right) {
    if (g_quiet) return;
    int width = getConsoleWidth();
    std::cout << color(left, dim);
    for (int i = 0; i < width - 2; ++i) {
        std::cout << color(fill, dim);
    }
    std::cout << color(right, dim) << '\n';
}

void drawFooter() {
    if (g_quiet) return;
    separator('=');
    std::cout << tagSuccess() << " " << color("Session MyZone terminee.", greenBright) << '\n';
}

void banner() {
    if (g_quiet) return;
    std::cout << '\n';
    std::cout << color("  __  __       ____                  ", greenBright) << '\n';
    std::cout << color(" |  \\/  |_   _|__  /___  _ __   ___  ", greenBright) << " " << color("v1.0", cyanBright) << '\n';
    std::cout << color(" | |\\/| | | | | / // _ \\| '_ \\ / _ \\ ", greenBright) << '\n';
    std::cout << color(" | |  | | |_| |/ /| (_) | | | |  __/ ", greenBright) << " " << color("local network discovery & device auditor", dim) << '\n';
    std::cout << color(" |_|  |_|\\__, /____\\___/|_| |_|\\___| ", greenBright) << " " << color("https://github.com/VISCHENZISCH/MyZone", cyan) << '\n';
    std::cout << color("         |___/                       ", greenBright) << '\n';
    separator('=');
}

void section(const std::string& text) {
    if (g_quiet) return;
    std::cout << '\n' << tagSuccess() << " " << color(text, whiteBright) << '\n';
}

void title(const std::string& text) {
    if (g_quiet) return;
    std::cout << '\n' << tagSuccess() << " " << color(text, whiteBright) << '\n';
}

void success(const std::string& text) {
    if (g_quiet) return;
    std::cout << tagSuccess() << " " << text << '\n';
}

void warning(const std::string& text) {
    if (g_quiet) return;
    std::cout << tagWarning() << " " << color(text, yellow) << '\n';
}

void error(const std::string& text) {
    if (g_quiet) return;
    std::cout << tagError() << " " << color(text, redBright) << '\n';
}

void info(const std::string& text) {
    if (g_quiet) return;
    std::cout << tagInfo() << " " << text << '\n';
}

void item(const std::string& text) {
    if (g_quiet) return;
    std::cout << "    [-] " << text << '\n';
}

std::string prompt(const std::string& label) {
    std::cout << tagPrompt() << " " << color(label, whiteBright) << std::flush;
    std::string value;
    std::getline(std::cin, value);
    return value;
}

void waitForEnter() {
    if (g_quiet) return;
    std::cout << '\n' << tagPrompt() << " " << color("Appuyez sur Entree pour continuer...", dim) << std::flush;
    std::string ignored;
    std::getline(std::cin, ignored);
}

} // namespace ui
} // namespace myzone
