#pragma once

#include <string>

namespace myzone {
namespace ui {

// Colors
constexpr const char* reset = "\033[0m";
constexpr const char* bold = "\033[1m";
constexpr const char* dim = "\033[2m";
constexpr const char* cyan = "\033[36m";
constexpr const char* blue = "\033[34m";
constexpr const char* green = "\033[32m";
constexpr const char* yellow = "\033[33m";
constexpr const char* red = "\033[31m";
constexpr const char* magenta = "\033[35m";

// Box drawing characters
constexpr const char* boxTopLeft = "┌";
constexpr const char* boxTopRight = "┐";
constexpr const char* boxBottomLeft = "└";
constexpr const char* boxBottomRight = "┘";
constexpr const char* boxHorizontal = "─";
constexpr const char* boxVertical = "│";
constexpr const char* boxDividerLeft = "├";
constexpr const char* boxDividerRight = "┤";

void initConsole();

bool colorsEnabled();
std::string color(const std::string& text, const char* colorCode);

// Layout functions
int getConsoleWidth();
void drawLine(const char* left, const char* fill, const char* right);
void drawFooter();
void printMenu();

// Helpers
void title(const std::string& text);
void success(const std::string& text);
void warning(const std::string& text);
void error(const std::string& text);

std::string prompt(const std::string& label);
void waitForEnter();

} // namespace ui
} // namespace myzone
