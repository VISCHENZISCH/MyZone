#include "../include/UI.hpp"
#include <cstdlib>
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

namespace {

struct LogStyle {
    const char* icon;
    const char* label;
    const char* colorCode;
};

LogStyle styleFor(const LogLevel level) {
    switch (level) {
    case LogLevel::Startup:  return {"▸", "start",    green};
    case LogLevel::Info:     return {"ℹ", "info",     blue};
    case LogLevel::Pending:  return {"□", "pending",  magenta};
    case LogLevel::Success:  return {"✓", "success",  green};
    case LogLevel::Warning:  return {"…", "watching", yellow};
    case LogLevel::Error:    return {"■", "error",    red};
    case LogLevel::Complete: return {"⊠", "complete", cyan};
    }
    return {"ℹ", "info", blue};
}

std::string paddedLabel(const char* label) {
    constexpr std::size_t labelWidth = 8;
    std::string value(label);
    if (value.size() < labelWidth) {
        value.append(labelWidth - value.size(), ' ');
    }
    return value;
}

} // namespace

void log(const LogLevel level, const std::string& text) {
    const LogStyle style = styleFor(level);
    const std::string label = paddedLabel(style.label);
    std::cout << color(style.icon, style.colorCode) << "  "
              << color(label, style.colorCode)
              << "  " << text << '\n';
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

void separator() {
    std::cout << color("\\============================================================", blue) << '\n';
}

void drawFooter() {
    separator();
    log(LogLevel::Complete, "Session MyZone terminée.");
}

void title(const std::string& text) {
    log(LogLevel::Startup, text);
}

void success(const std::string& text) {
    log(LogLevel::Success, text);
}

void warning(const std::string& text) {
    log(LogLevel::Warning, text);
}

void error(const std::string& text) {
    log(LogLevel::Error, text);
}

std::string prompt(const std::string& label) {
    std::cout << color("▸", magenta) << "  "
              << color("input   ", magenta)
              << "  " << label << std::flush;
    std::string value;
    std::getline(std::cin, value);
    return value;
}

void waitForEnter() {
    std::cout << color("…", yellow) << "  "
              << color("watching", yellow)
              << "  Appuyez sur Entrée pour continuer..." << std::flush;
    std::string ignored;
    std::getline(std::cin, ignored);
}

void printMenu() {
    std::cout << '\n';
    separator();
    log(LogLevel::Info, "Tableau de bord MyZone");
    log(LogLevel::Pending, "[1] Rechercher un appareil par adresse MAC");
    log(LogLevel::Pending, "[0] Quitter");
}

} // namespace ui
} // namespace myzone
