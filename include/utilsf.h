#pragma once
#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <vector>
#include <algorithm>
#include <unordered_map>
#include <filesystem>
#include <cstdlib>
#include <iomanip>
#include <functional>
#include <cctype>
#include <variant>

namespace fs = std::filesystem;

namespace utilsf
{
    std::string escapeShellArg(const std::string& arg);
    bool ends_with(const std::string& str, const std::string& suffix);
    bool start_with(const std::string& str, const std::string& prefix);
    std::vector<std::string> split(const std::string& text, char delimiter);
    std::string strip(const std::string& s);

}