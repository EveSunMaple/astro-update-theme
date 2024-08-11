#ifndef CHECK_H
#define CHECK_H

#include <filesystem>
#include <fstream>
#include <sstream>
#include <string>
#include <regex>
#include <nlohmann/json.hpp>

namespace fs = std::filesystem;
using Json = nlohmann::json;

// 检查两个文件内容是否相同
bool areFilesIdentical(const fs::path &file1, const fs::path &file2);

// 检查此文件是否不需要更改
bool isNoChangeFile(const fs::path &filePath);

// 检查此文件是否不需要保留
bool canDeleteFile(const fs::path &filePath);

// 检查此文件是否可以被直接覆盖（通过目录审查）
bool canOverwriteFileByDirectory(const fs::path &filePath);

// 检查此文件是否可以被直接覆盖（通过文件内容）
bool canOverwriteFileByContent(const fs::path &filePath, const fs::path &compareFilePath, std::vector<std::string> &log);

#endif // CHECK_H
