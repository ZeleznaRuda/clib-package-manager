#pragma once
#include <filesystem>
#include <string>
#include <cstdlib>

namespace fs = std::filesystem;

inline fs::path getUserHomeDirectory() {
    const char* home = getenv("HOME");
    return home ? fs::path(home) : fs::current_path();
}

inline std::string version = "1.1.8";
inline fs::path homeDirectory = getUserHomeDirectory() / ".clibx";
inline fs::path userHomeDirectory = getUserHomeDirectory();
