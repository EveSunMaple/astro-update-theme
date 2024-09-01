#include "utils/colors.hpp"
#include "utils/config.h"
#include "utils/message.hpp"

int main()
{
    MessagePrinter printer("messages.json");
    printer.print("welcome", colorize("INFO", "cyan"), colorize("Astro Project!", "yellow"));
    printer.print("file_not_found", colorize("include/termcolor", "cyan"), colorize("termcolor.hpp", "cyan"));

    CheckDependency("git --version");
    CheckDependency("node -v");

    std::string configFile = "config.json";
    Json config;

    if (!ConfigExists(configFile))
    {
        PRINT_CYAN("Configuration file not found. Creating a new configuration file.\n");
        CreateConfig(configFile);
        PRINT_GREEN("Configuration file has been created and saved as " << TEXT_CYAN(configFile) << TEXT_GREEN(".\n"));
    }
    else
    {
        PRINT_GREEN("Configuration file " << TEXT_CYAN(configFile) << TEXT_GREEN(" already exists.\n"));
        std::ifstream file(configFile);
        file >> config;
        file.close();
        if (!ValidateConfig(config))
            return 1;
        system("pause");
    }

    return 0;
}
