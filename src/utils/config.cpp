#include <iostream>
#include <cstdlib>
#include "utils/config.h"
#include "utils/message.hpp"

void ClearScreen()
{
#ifdef _WIN32
    system("cls");
#else
    system("clear");
#endif
}

void CheckDependency(const std::string &command)
{

    MessagePrinter printer("messages.json");
    int result = std::system(command.c_str());
    if (result != 0)
        printer.print("check_dependency_deficiency", "ERROR", command);
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

    std::cin >> name;
    std::cin >> localRepo;
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

    isValid = nameExists;
    return isValid;
}