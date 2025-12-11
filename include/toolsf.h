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

int sysf(const std::vector<std::string>& argv);

namespace stringf
{
    std::string escape(const std::string& arg);
    bool starts_with(const std::string& str, const std::string& prefix);
    bool ends_with(const std::string& str, const std::string& suffix);
    std::vector<std::string> split(const std::string& text, char delimiter);
    std::string strip(const std::string& s);
    std::string join(const std::vector<std::string>& vec, const std::string& delim);

} // namespace stringf
