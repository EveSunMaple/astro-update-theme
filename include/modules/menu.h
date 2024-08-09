#ifndef MENU_HPP
#define MENU_HPP

#include <string>
#include <fstream>
#include <nlohmann/json.hpp>

using Json = nlohmann::json;

void DisplayAllProjects(const Json &config);
void DisplayProjects(const Json &config);
void DisplayMenu(int projectCount);
void EnterMenu(Json &config, std::string configFile);

#endif // MENU_HPP
