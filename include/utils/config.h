#ifndef CONFIG_UTILS_HPP
#define CONFIG_UTILS_HPP

#include <string>
#include <fstream>
#include <nlohmann/json.hpp>

using Json = nlohmann::json;

void ClearScreen();
bool CheckDependency(const std::string &command);
bool ConfigExists(const std::string &filename);
void CreateConfig(const std::string &filename);
bool ValidateConfig(const Json &config);

#endif // CONFIG_UTILS_HPP
