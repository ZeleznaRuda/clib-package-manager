#pragma once
#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
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

#include "../include/toolsf.h"
#include "../include/clif.h"

namespace fs = std::filesystem;

namespace yaml
{
    std::string read(const fs::path& fileName);
    std::unordered_map<std::string, std::variant<std::string, std::vector<std::string>>> parser(const std::string& fileContent);
}
