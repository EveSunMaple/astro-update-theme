#ifndef OPERATION_H
#define OPERATION_H

#include <filesystem>
#include <iostream>
#include <fstream>
#include <cstdlib>
#include <vector>
#include <string>
#include <algorithm>
#include <unordered_set>
#include <nlohmann/json.hpp>
#include <progresscpp/ProgressBar.hpp>
#include <nlohmann/json.hpp>

using Json = nlohmann::json;

// 更改当前选择的项目
void ChangeProject(Json &config);

// 新建一个项目
void CreateNewProject(Json &config);

// 更新项目
void CheckForUpdates(Json &config);

#endif // OPERATION_H
