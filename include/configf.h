#pragma once
#include "../include/parsers.h"
#include "../include/transactionf.h"
#include "../include/constants.h"

namespace configf{
std::string loadConfig(const std::string& key);
void changeConfig(const std::string& key, const std::string& content);
}