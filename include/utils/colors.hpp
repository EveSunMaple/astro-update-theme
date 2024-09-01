#ifndef COLOR_MACROS_HPP
#define COLOR_MACROS_HPP

#include <termcolor/termcolor.hpp>

#define PRINT_YELLOW(msg) std::cout << termcolor::yellow << msg << termcolor::reset
#define PRINT_RED(msg) std::cerr << termcolor::red << msg << termcolor::reset
#define PRINT_CYAN(msg) std::cout << termcolor::cyan << msg << termcolor::reset
#define PRINT_GREEN(msg) std::cout << termcolor::green << msg << termcolor::reset
#define PRINT_GREY(msg) std::cout << termcolor::bright_grey << msg << termcolor::reset

#define TEXT_YELLOW(msg) termcolor::yellow << msg << termcolor::reset
#define TEXT_RED(msg) termcolor::red << msg << termcolor::reset
#define TEXT_CYAN(msg) termcolor::cyan << msg << termcolor::reset
#define TEXT_GREEN(msg) termcolor::green << msg << termcolor::reset
#define TEXT_GREY(msg) termcolor::bright_grey << msg << termcolor::reset

#include <unordered_map>

// ANSI 转义码映射表
const std::unordered_map<std::string, std::string> colorCodes = {
    {"normal", "\033[0m"},
    {"red", "\033[31m"},
    {"green", "\033[32m"},
    {"yellow", "\033[33m"},
    {"blue", "\033[34m"},
    {"magenta", "\033[35m"},
    {"cyan", "\033[36m"},
    {"white", "\033[37m"},
    {"grey", "\033[90m"}, // 亮灰色
    {"bright_red", "\033[91m"},
    {"bright_green", "\033[92m"},
    {"bright_yellow", "\033[93m"},
    {"bright_blue", "\033[94m"},
    {"bright_magenta", "\033[95m"},
    {"bright_cyan", "\033[96m"},
    {"bright_white", "\033[97m"},
};

// 函数：根据颜色返回带有颜色的文本
std::string colorize(const std::string &text, const std::string &color)
{
    auto it = colorCodes.find(color);
    if (it != colorCodes.end())
    {
        return it->second + text + "\033[0m";
    }
    else
    {
        return text; // 如果颜色无效，则返回原文本
    }
}
#endif // COLOR_MACROS_HPP