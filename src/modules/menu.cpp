#include <iostream>
#include <fstream>
#include <cstdlib>
#include <nlohmann/json.hpp>
#include "utils/colors.h"
#include "utils/config.h"
#include "modules/menu.h"
#include "modules/operation.h"

void DisplayAllProjects(const Json &config)
{
    std::cout << "All Projects:\n\n";
    for (const auto &project : config["astro-project"])
    {
        PRINT_CYAN(project["name"]) << "\n";
    }
    std::cout << "\n";
    return;
}

void DisplayProjects(const Json &config)
{
    std::cout << "Current Project:\n\n";

    for (const auto &project : config["astro-project"])
    {
        if (project["name"] == config["name"])
        {
            PRINT_GREY("Name: " << TEXT_CYAN(project["name"]) << "\n");
            PRINT_GREY("Local Repo: " << TEXT_CYAN(project["local_repo"]) << "\n");
            PRINT_GREY("Clone Repo: " << TEXT_CYAN(project["clone_repo"]) << "\n");
            PRINT_GREY("Remote Repo: " << TEXT_CYAN(project["remote_repo"]) << "\n");
            std::cout << "\n";
            return;
        }
    }

    PRINT_RED("Project not found.\n");
}

void DisplayMenu(int projectCount)
{
    std::cout << "Select an operation:\n";
    if (projectCount > 1)
    {
        std::cout << "1. Change Project\n";
    }
    else
    {
        std::cout << "1. " << TEXT_GREY("Change Project (Disabled)\n");
    }
    std::cout << "2. Create New Project\n";
    std::cout << "3. Check for Updates\n";
}

void EnterMenu(Json &config, std::string configFile)
{
    while (true)
    {
        ClearScreen();
        DisplayProjects(config);
        DisplayMenu(config["astro-project"].size());
        std::cout << "Enter your choice (1-3): \n";
        int choice = 0;
        std::cin >> choice;
        switch (choice)
        {
        case 1:
            if (config["astro-project"].size() > 1)
            {
                ClearScreen();
                DisplayAllProjects(config);
                ChangeProject(config);
            }
            else
            {
                PRINT_RED("Change Project option is disabled as there's only one project.\n");
            }
            break;
        case 2:
            ClearScreen();
            CreateNewProject(config);
            break;
        case 3:
            ClearScreen();
            CheckForUpdates(config);
            break;
        default:
            PRINT_RED("Invalid choice. Please select between 1 and 3.\n");
            break;
        }
        system("pause");
        std::ofstream file(configFile);
        file << config.dump(2);
    }
}