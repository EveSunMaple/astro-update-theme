#include "modules/check.h"

// 读取文件内容
std::string readFileContent(const fs::path &filePath)
{
    std::ifstream file(filePath, std::ios::binary);
    return {std::istreambuf_iterator<char>(file), std::istreambuf_iterator<char>()};
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

// 检查此文件是否可以被直接覆盖（通过文件内容）
bool canOverwriteFileByContent(const fs::path &filePath)
{
    //
    return true;
}