#include <filesystem>
#include <iostream>
#include <fstream>
#include <cstdlib>
#include <vector>
#include <nlohmann/json.hpp>
#include <progresscpp/ProgressBar.hpp>
#include "utils/colors.h"
#include "utils/config.h"
#include "modules/operation.h"
namespace fs = std::filesystem;

void CloneRepoAndIgnore(const std::string &cloneDir, const std::string &remoteRepo)
{
    std::string cloneCommand = "git clone " + remoteRepo + " " + cloneDir;
    if (std::system(cloneCommand.c_str()) == 0)
    {
        PRINT_GREEN("Successfully cloned the repository.\n");
    }
    else
    {
        PRINT_RED("Failed to clone the repository.\n");
    }
}

void PullRepo(const std::string &cloneDir)
{
    std::string pullCommand = "cd " + cloneDir + " && git pull -f";
    if (std::system(pullCommand.c_str()) == 0)
    {
        PRINT_GREEN("Successfully pulled the latest changes.\n");
    }
    else
    {
        PRINT_RED("Failed to pull the repository.\n");
    }
}

std::vector<std::string> CompareDirectories(const fs::path &dir1, const fs::path &dir2)
{
    std::vector<std::string> differences;

    size_t totalFiles = 0;
    for (const auto &entry : fs::recursive_directory_iterator(dir1))
    {
        if (entry.path().string().find("node_modules") == std::string::npos && entry.path().string().find("\\.") == std::string::npos)
        {
            ++totalFiles;
        }
    }

    progresscpp::ProgressBar progressBar(totalFiles, 50);
    PRINT_CYAN("Comparing directories...\n");

    for (auto &entry : fs::recursive_directory_iterator(dir1))
    {
        if (entry.path().string().find("node_modules") != std::string::npos || entry.path().string().find("\\.") != std::string::npos)
        {
            continue;
        }

        auto relativePath = fs::relative(entry.path(), dir1);
        auto correspondingPath = dir2 / relativePath;

        if (!fs::exists(correspondingPath))
        {
            differences.push_back(relativePath.string());
        }

        ++progressBar;
        progressBar.display();
    }

    progressBar.done();

    return differences;
}

void ChangeProject(Json &config)
{
    std::cout << "Enter the name of the project you want to select:\n";
    while (true)
    {
        std::string projectName;
        std::cin >> projectName;
        config["name"] = projectName;
        if (ValidateConfig(config))
        {
            PRINT_GREEN("You have chosen a new project.\n");
            return;
        }
    }
    return;
}

void CreateNewProject(Json &config)
{
    std::string name, localRepo, cloneRepo, remoteRepo;

    PRINT_YELLOW("Enter Blog name: ");
    std::cin >> name;
    PRINT_YELLOW("Enter local repository URL: ");
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
}

void CheckForUpdates(Json &config)
{
    std::string currentProjectName = config["name"];
    Json currentProject;

    for (const auto &project : config["astro-project"])
    {
        if (project["name"] == currentProjectName)
        {
            currentProject = project;
            break;
        }
    }

    if (currentProject.empty())
    {
        PRINT_RED("No matching project found in the configuration.\n");
        return;
    }

    std::string localRepo = currentProject["local_repo"];
    std::string remoteRepo = currentProject["remote_repo"];
    std::string cloneDir = localRepo + "_clone";

    if (!fs::exists(cloneDir))
    {
        PRINT_CYAN("Clone directory does not exist. Cloning the repository...\n");
        CloneRepoAndIgnore(cloneDir, remoteRepo);
    }
    else
    {
        PRINT_CYAN("Clone directory exists. Pulling the latest changes...\n");
        PullRepo(cloneDir);
    }

    PRINT_CYAN("Scanning directories...\n");
    std::vector<std::string> differences = CompareDirectories(fs::path(localRepo), fs::path(cloneDir));

    if (differences.empty())
    {
        PRINT_GREEN("No differences found between the original and cloned repositories.\n");
    }
    else
    {
        PRINT_CYAN("Differences found:\n");
        for (const auto &diff : differences)
        {
            std::cout << diff << "\n";
        }
    }
}