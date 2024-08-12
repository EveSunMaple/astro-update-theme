#include <iostream>
#include <cstdlib>
#include "utils/colors.h"
#include "utils/config.h"

void ClearScreen()
{
#ifdef _WIN32
    system("cls");
#else
    system("clear");
#endif
}

bool CheckDependency(const std::string &command)
{
    int result = std::system(command.c_str());
    return result == 0;
}

bool ConfigExists(const std::string &filename)
{
    std::ifstream file(filename);
    return file.good();
}

void CreateConfig(const std::string &filename)
{
    Json config;
    config["astro-project"] = Json::array();

    std::string name, localRepo, cloneRepo, remoteRepo;

    PRINT_YELLOW("Enter Blog name: ");
    std::cin >> name;
    PRINT_YELLOW("Enter local repository path: ");
    std::cin >> localRepo;
    PRINT_YELLOW("Enter remote repository URL: ");
    std::cin >> remoteRepo;

    cloneRepo = localRepo;

    Json blog;
    blog["name"] = name;
    blog["local_repo"] = localRepo;
    blog["clone_repo"] = cloneRepo;
    blog["remote_repo"] = remoteRepo;

    config["astro-project"].push_back(blog);
    config["name"] = name;

    std::ofstream file(filename);
    file << config.dump(2);
}

bool ValidateConfig(const Json &config)
{
    bool isValid = true;
    std::string mainName = config["name"];
    bool nameExists = false;

    for (const auto &project : config["astro-project"])
    {
        if (project["name"] == mainName)
        {
            nameExists = true;
            break;
        }
    }

    if (!nameExists)
        PRINT_RED("There is no project named " << TEXT_CYAN(mainName << ".\n"));

    isValid = nameExists;
    return isValid;
}