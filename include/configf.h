#pragma once
#include "../include/parsers.h"
#include "../include/core.h"
namespace configf{
std::string loadConfig(const std::string& key);
void changeConfig(const std::string& key, const std::string& content);
}