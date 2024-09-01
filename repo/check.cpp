#include "modules/check.h"

// 读取文件内容
std::string readFileContent(const fs::path &filePath)
{
    std::ifstream file(filePath);
    std::string content{std::istreambuf_iterator<char>(file), std::istreambuf_iterator<char>()};
    file.close();
    return content;
}

// 检查两个文件内容是否相同
bool areFilesIdentical(const fs::path &file1, const fs::path &file2)
{
    return readFileContent(file1) == readFileContent(file2);
}

// 检查此文件是否不需要更改
bool isNoChangeFile(const fs::path &filePath)
{
    auto parent = filePath.parent_path().string();
    auto extension = filePath.extension().string();
    return (parent.find("public") != std::string::npos || extension == ".md" || extension == ".mdx");
}

// 检查此文件是否不需要保留
bool canDeleteFile(const fs::path &filePath)
{
    auto parent = filePath.parent_path().string();
    auto extension = filePath.extension().string();
    return ((parent.find("scripts") != std::string::npos) &&
            (extension == ".ts" || extension == ".js" ||
             extension == ".mjs" || extension == ".cjs" ||
             extension == ".css" || extension == ".scss" ||
             extension == ".yaml" || extension == ".json"));
}

// 检查此文件是否可以被直接覆盖（通过目录审查）
bool canOverwriteFileByDirectory(const fs::path &filePath)
{
    auto parent = filePath.parent_path().string();
    auto extension = filePath.extension().string();
    return ((parent.find("src") != std::string::npos ||
             parent.find("scripts") != std::string::npos) &&
            (extension == ".ts" || extension == ".js" ||
             extension == ".mjs" || extension == ".cjs" ||
             extension == ".css" || extension == ".scss" ||
             extension == ".yaml" || extension == ".json"));
}
bool canOverwriteFileByContent(const fs::path &filePath, const fs::path &compareFilePath, std::vector<std::string> &log)
{
    // 打开两个文件
    std::ifstream file1(filePath);
    std::ifstream file2(compareFilePath);

    // 检查文件是否成功打开
    if (!file1.is_open())
    {
        log.push_back("Error: Unable to open file " + filePath.string());
        return false;
    }
    if (!file2.is_open())
    {
        log.push_back("Error: Unable to open file " + compareFilePath.string());
        return false;
    }

    std::string line1, line2;
    std::regex customPattern(R"(>([^{}<>]+)<)");
    std::smatch match;
    bool canOverwrite = true;

    // 逐行比较文件内容
    while (std::getline(file1, line1) && std::getline(file2, line2))
    {
        if (line1 != line2)
        {
            // 检查不同的行是否包含 ">字符串<"
            if (std::regex_search(line1, match, customPattern))
            {
                canOverwrite = false;
                log.push_back("Line in " + filePath.string() + " contains the pattern: " + line1);
            }
            if (std::regex_search(line2, match, customPattern))
            {
                canOverwrite = false;
                log.push_back("Line in " + compareFilePath.string() + " contains the pattern: " + line2);
            }
        }
    }

    // 如果一个文件比另一个文件有更多的行，继续处理剩余的行
    while (std::getline(file1, line1))
    {
        if (std::regex_search(line1, match, customPattern))
        {
            canOverwrite = false;
            log.push_back("Line in " + filePath.string() + " contains the pattern: " + line1);
        }
    }

    while (std::getline(file2, line2))
    {
        if (std::regex_search(line2, match, customPattern))
        {
            canOverwrite = false;
            log.push_back("Line in " + compareFilePath.string() + " contains the pattern: " + line2);
        }
    }

    // 关闭文件
    file1.close();
    file2.close();

    return canOverwrite;
}