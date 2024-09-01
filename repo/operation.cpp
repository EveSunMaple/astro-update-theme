#include "utils/colors.hpp"
#include "utils/config.h"
#include "modules/check.h"
#include "modules/operation.h"

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

void ClearDirectory(const fs::path &dir)
{
    try
    {
        if (fs::exists(dir))
        {
            fs::remove_all(dir);
        }
        fs::create_directories(dir);
    }
    catch (const std::exception &e)
    {
        PRINT_RED("Error clearing directory: " << e.what() << "\n");
    }
}

std::vector<std::string> CompareDirectories(const fs::path &dir1, const fs::path &dir2)
{
    std::vector<std::string> differences;
    std::unordered_set<std::string> allPaths;
    const std::unordered_set<std::string> exclude = {"node_modules", "dist", "astro.config.js", "astro.config.mjs"};

    auto shouldExclude = [&](const fs::path &path)
    {
        return path.filename().string().starts_with(".") || exclude.find(path.filename().string()) != exclude.end();
    };

    try
    {
        for (auto it = fs::recursive_directory_iterator(dir1); it != fs::recursive_directory_iterator(); ++it)
        {
            if (shouldExclude(it->path()))
            {
                it.disable_recursion_pending();
            }
            else
            {
                auto relativePath = fs::relative(it->path(), dir1).string();
                allPaths.insert(relativePath);
            }
        }
    }
    catch (const std::exception &e)
    {
        PRINT_RED("Error iterating directory: " << e.what() << "\n");
    }

    PRINT_CYAN("Backing up files...\n");

    fs::path backupDir = dir1.parent_path() / (dir1.filename().string() + "_backup");
    ClearDirectory(backupDir);

    for (const auto &relativePath : allPaths)
    {
        fs::path sourcePath = dir1 / relativePath;
        fs::path backupPath = backupDir / relativePath;

        try
        {
            if (fs::is_regular_file(sourcePath))
            {
                fs::create_directories(backupPath.parent_path());
                fs::copy_file(sourcePath, backupPath, fs::copy_options::overwrite_existing);
            }
        }
        catch (const std::exception &e)
        {
            PRINT_RED("Error backing up file: " << e.what() << "\n");
        }
    }

    try
    {
        for (auto it = fs::recursive_directory_iterator(dir2); it != fs::recursive_directory_iterator(); ++it)
        {
            if (shouldExclude(it->path()))
            {
                it.disable_recursion_pending();
            }
            else
            {
                auto relativePath = fs::relative(it->path(), dir2).string();
                allPaths.insert(relativePath);
            }
        }
    }
    catch (const std::exception &e)
    {
        PRINT_RED("Error iterating directory: " << e.what() << "\n");
    }

    progresscpp::ProgressBar progressBar(allPaths.size(), 50);
    PRINT_CYAN("Comparing directories...\n");

    for (const auto relativePath : allPaths)
    {
        auto pathInDir1 = dir1 / relativePath;
        auto pathInDir2 = dir2 / relativePath;

        try
        {
            if (fs::exists(pathInDir1) && !fs::exists(pathInDir2))
            {
                differences.push_back("\e[0;31m[local only] \e[0m" + relativePath);
            }
            else if (!fs::exists(pathInDir1) && fs::exists(pathInDir2))
            {
                differences.push_back("\e[0;32m[clone only] \e[0m" + relativePath);
            }
            else if (fs::exists(pathInDir1) && fs::exists(pathInDir2))
            {
                if (!areFilesIdentical(pathInDir1, pathInDir2))
                    differences.push_back("\e[0;33m[diff files] \e[0m" + relativePath);
            }
        }
        catch (const std::exception &e)
        {
            PRINT_RED("Error comparing files: " << e.what() << "\n");
        }

        ++progressBar;
        progressBar.display();
    }

    progressBar.done();
    std::sort(differences.begin(), differences.end(), [](const std::string &a, const std::string &b)
              {
        auto getPriority = [](const std::string& str) {
            if (str.find("[local only]") != std::string::npos) return 1;
            if (str.find("[clone only]") != std::string::npos) return 2;
            if (str.find("[diff files]") != std::string::npos) return 3;
            return 4;
        };
        return getPriority(a) < getPriority(b); });
    return differences;
}

std::vector<std::string> AnalyzeFiles(const fs::path &dir1, const fs::path &dir2, const std::vector<std::string> &diff, std::vector<std::string> &log)
{
    size_t totalDelete = 0, totalAdd = 0, totalOverwrite = 0;
    std::vector<std::string> report;
    progresscpp::ProgressBar progressBar(diff.size(), 50);
    for (auto relativePath : diff)
    {
        int operation = 0;
        if (relativePath.find("[local only]") != std::string::npos)
            operation = 1;
        if (relativePath.find("[clone only]") != std::string::npos)
            operation = 2;
        if (relativePath.find("[diff files]") != std::string::npos)
            operation = 3;
        relativePath.erase(0, 24);
        fs::path pathInDir1 = dir1 / relativePath;
        fs::path pathInDir2 = dir2 / relativePath;

        if (operation == 1 && canDeleteFile(pathInDir1))
        {
            fs::remove(pathInDir1);
            report.push_back("\e[0;31mD \e[0m" + relativePath);
            totalDelete++;
        }
        if (operation == 2 && !isNoChangeFile(pathInDir2))
        {
            if (!fs::exists(pathInDir1))
                fs::create_directories(pathInDir1.parent_path());
            fs::copy(pathInDir2, pathInDir1, fs::copy_options::overwrite_existing);
            report.push_back("\e[0;32mA \e[0m" + relativePath);
            totalAdd++;
        }
        if (operation == 3 && !isNoChangeFile(pathInDir1))
        {
            auto parent = pathInDir1.parent_path().filename().string();
            auto extension = pathInDir1.extension().string();
            if (extension == ".astro")
            {
                if (canOverwriteFileByContent(pathInDir1, pathInDir2, log))
                {
                    report.push_back("\e[0;36mO \e[0m" + relativePath);
                    if (fs::exists(pathInDir1))
                        fs::remove(pathInDir1);
                    fs::copy(pathInDir2, pathInDir1, fs::copy_options::overwrite_existing);
                    totalOverwrite++;
                }
                else
                {
                    report.push_back("\e[0;35mI \e[0mCannot overwrite file\e[0;36m " + relativePath + " \e[0mas it may have stored some content related to your blog.");
                }
            }
            else if (parent.find("src") != std::string::npos)
            {
                report.push_back("\e[0;35mI \e[0mCannot overwrite file\e[0;36m " + relativePath + " \e[0mas it may be a configuration file, please check it yourself.");
            }
            else
            {
                if (canOverwriteFileByDirectory(pathInDir1) || relativePath.find("\\") == std::string::npos)
                {
                    report.push_back("\e[0;36mO \e[0m" + relativePath);
                    if (fs::exists(pathInDir1))
                        fs::remove(pathInDir1);
                    fs::copy(pathInDir2, pathInDir1, fs::copy_options::overwrite_existing);
                    totalOverwrite++;
                }
            }
        }

        ++progressBar;
        progressBar.display();
    }
    std::sort(report.begin(), report.end(), [](const std::string &a, const std::string &b)
              {
        auto getPriority = [](const std::string& str) {
            if (str.find("\e[0;31mD \e[0m") != std::string::npos) return 1;
            if (str.find("\e[0;32mA \e[0m") != std::string::npos) return 2;
            if (str.find("\e[0;36mO \e[0m") != std::string::npos) return 3;
            if (str.find("\e[0;35mI \e[0m") != std::string::npos) return 4;
            return 4;
        };
        return getPriority(a) < getPriority(b); });
    report.push_back("Deleted \e[0;36m" + std::to_string(totalDelete) + " \e[0mfiles, added \e[0;36m" + std::to_string(totalAdd) + " \e[0mfiles while overwriting \e[0;36m" + std::to_string(totalOverwrite) + " \e[0mfiles");
    progressBar.done();
    return report;
}

void runPackageManagerUpdate(const fs::path &dirPath)
{
    bool hasPnpmLock = fs::exists(dirPath / "pnpm-lock.yaml");
    bool hasYarnLock = fs::exists(dirPath / "yarn.lock");

    std::string command = "cd " + dirPath.string() + " && npm install";
    if (hasPnpmLock)
    {
        PRINT_CYAN("pnpm-lock.yaml found. Running 'pnpm update'...\n");
        command = "cd " + dirPath.string() + " && pnpm update";
        if (std::system(command.c_str()) != 0)
        {
            PRINT_RED("Failed to run 'pnpm update'\n");
        }
    }

    if (hasYarnLock)
    {
        PRINT_CYAN("yarn.lock found. Running 'yarn install'...\n");
        command = "cd " + dirPath.string() + " && yarn install";
        if (std::system(command.c_str()) != 0)
        {
            PRINT_RED("Failed to run 'yarn install'\n");
        }
    }

    PRINT_CYAN("Running 'npm install'...\n");
    command = "cd " + dirPath.string() + " && npm install";
    if (std::system(command.c_str()) != 0)
    {
        PRINT_RED("Failed to run 'pnpm install'\n");
    }

    return;
}

// 下面的函数可以被 menu 调用

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
        return;
    }

    PRINT_CYAN("Differences found:\n");
    for (const auto &diff : differences)
    {
        std::cout << diff << "\n";
    }

    PRINT_CYAN("Analyzing files...\n");
    std::vector<std::string> log;
    std::vector<std::string> report = AnalyzeFiles(fs::path(localRepo), fs::path(cloneDir), differences, log);
    for (const auto &rpt : report)
    {
        std::cout << rpt << "\n";
    }
    for (const auto &l : log)
    {
        std::cout << l << "\n";
    }

    runPackageManagerUpdate(fs::path(localRepo));
    return;
}