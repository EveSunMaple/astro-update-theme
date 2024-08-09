#ifndef OPERATION_HPP
#define OPERATION_HPP

#include <string>
#include <fstream>
#include <nlohmann/json.hpp>

using Json = nlohmann::json;

void ChangeProject(Json &config);
void CreateNewProject(Json &config);
void CheckForUpdates(Json &config);

#endif // OPERATION_HPP
