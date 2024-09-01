#ifndef CONFIG_UTILS_H
#define CONFIG_UTILS_H

#include <iostream>
#include <fstream>
#include <cstdlib>
#include <nlohmann/json.hpp>

using Json = nlohmann::json;

// 清屏
void ClearScreen();

// 检查依赖
void CheckDependency(const std::string &command);

// 检查配置文件是否存在
bool ConfigExists(const std::string &filename);

// 新键配置文件
void CreateConfig(const std::string &filename);

// 验证配置文件是否正确
bool ValidateConfig(const Json &config);

#endif // CONFIG_UTILS_H
