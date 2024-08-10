#include "utils/colors.h"
#include "utils/config.h"
#include "modules/menu.h"

int main()
{
    if (!CheckDependency("git --version"))
    {
        PRINT_RED("Git is not installed. Please install Git and try again.\n");
        return 1;
    }

    if (!CheckDependency("node -v"))
    {
        PRINT_RED("Node.js is not installed. Please install Node.js and try again.\n");
        return 1;
    }

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
        EnterMenu(config, configFile);
    }

    return 0;
}
