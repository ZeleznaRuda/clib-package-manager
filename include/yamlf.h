#pragma once
#include <iostream>
#include <fstream>
#include <sstream>
#include <regex>
#include <vector>
#include <algorithm>
#include <unordered_map>
#include <filesystem>
#include <cstdlib>
#include <iomanip>
#include <functional>
#include <cctype>
#include <variant>
#include <string>

#include "../include/toolsf.h"
#include "../include/clif.h"

namespace fs = std::filesystem;

typedef std::unordered_map<std::string, std::variant<std::string, std::vector<std::string>>> yaml_t;

namespace yaml
{
    std::string read(const fs::path& fileName);
    yaml_t parser(const std::string& fileContent);
}
