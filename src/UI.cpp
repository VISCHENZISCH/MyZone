#include "../include/UI.hpp"
#include <cstdlib>
#include <iostream>
#include <iomanip>
#include <string>
#include <chrono>
#include <ctime>

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

void clearConsole() {
#ifdef _WIN32
    std::system("cls");
#else
    std::system("clear");
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
    const char* colorCode;
};

LogStyle styleFor(const LogLevel level) {
    switch (level) {
    case LogLevel::Startup:  return {"●", green};
    case LogLevel::Info:     return {"i", blue};
    case LogLevel::Pending:  return {"·", dim};
    case LogLevel::Success:  return {"✓", green};
    case LogLevel::Warning:  return {"!", yellow};
    case LogLevel::Error:    return {"x", red};
    case LogLevel::Complete: return {"●", cyan};
    }
    return {"·", dim};
}

std::string currentTime() {
    auto now = std::chrono::system_clock::now();
    std::time_t time = std::chrono::system_clock::to_time_t(now);
    std::tm tm_buf;
#ifdef _WIN32
    localtime_s(&tm_buf, &time);
#else
    localtime_r(&time, &tm_buf);
#endif
    char buf[10];
    std::strftime(buf, sizeof(buf), "%H:%M:%S", &tm_buf);
    return std::string(buf);
}

} // namespace

void log(const LogLevel level, const std::string& text) {
    const LogStyle style = styleFor(level);
    std::cout << " " << color(currentTime(), dim) 
              << "  " << color(style.icon, style.colorCode)
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
    std::cout << color(std::string(75, '='), cyan) << '\n';
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

void info(const std::string& text) {
    log(LogLevel::Info, text);
}

void item(const std::string& text) {
    log(LogLevel::Pending, text);
}

std::string prompt(const std::string& label) {
    std::cout << " " << color("➤", cyan) << " " << label << std::flush;
    std::string value;
    std::getline(std::cin, value);
    return value;
}

void waitForEnter() {
    std::cout << " " << color("➤", yellow) << " Appuyez sur Entrée pour continuer..." << std::flush;
    std::string ignored;
    std::getline(std::cin, ignored);
}

} // namespace ui
} // namespace myzone
