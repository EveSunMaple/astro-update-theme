#ifndef COLOR_MACROS_HPP
#define COLOR_MACROS_HPP

#include <termcolor/termcolor.hpp>

#define PRINT_YELLOW(msg) std::cout << termcolor::yellow << msg << termcolor::reset
#define PRINT_RED(msg) std::cerr << termcolor::red << msg << termcolor::reset
#define PRINT_CYAN(msg) std::cout << termcolor::cyan << msg << termcolor::reset
#define PRINT_GREEN(msg) std::cout << termcolor::green << msg << termcolor::reset
#define PRINT_GREY(msg) std::cout << termcolor::grey << msg << termcolor::reset

#define TEXT_YELLOW(msg) termcolor::yellow << msg << termcolor::reset
#define TEXT_RED(msg) termcolor::red << msg << termcolor::reset
#define TEXT_CYAN(msg) termcolor::cyan << msg << termcolor::reset
#define TEXT_GREEN(msg) termcolor::green << msg << termcolor::reset
#define TEXT_GREY(msg) termcolor::grey << msg << termcolor::reset

#endif // COLOR_MACROS_HPP