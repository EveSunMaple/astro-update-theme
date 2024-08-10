#ifndef MENU_H
#define MENU_H

#include <iostream>
#include <fstream>
#include <cstdlib>
#include <nlohmann/json.hpp>

using Json = nlohmann::json;

// 显示所有的项目
void DisplayAllProjects(const Json &config);

// 显示当前的项目
void DisplayProjects(const Json &config);

// 显示菜单
void DisplayMenu(int projectCount);

// 控制菜单
void EnterMenu(Json &config, std::string configFile);

#endif // MENU_H
