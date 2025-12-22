#pragma once
#include <filesystem>
#include <string>
#include <cstdlib>

namespace fs = std::filesystem;

inline constexpr std::string_view VERSION = "4.1.0";
inline const     fs::path HOME = fs::path(getenv("HOME"));
inline const     fs::path HOME_DIRECTORY = HOME / ".cclm";
inline const     fs::path CURRENT_PATH = fs::current_path();

inline const std::string LIBRARY_FILE = std::string(
    std::getenv("CCLM_LIBRARY_FILE_NAME") 
    ? std::getenv("CCLM_LIBRARY_FILE_NAME")  
    : ".library"
);
inline const std::string PROJECT_FILE = std::string(
    std::getenv("CCLM_PROJECT_FILE_NAME") 
    ? std::getenv("CCLM_PROJECT_FILE_NAME")  
    : ".project"
);
inline const fs::path GIT_PATH = fs::path(std::getenv("CCLM_GIT_PATH") ? std::getenv("CCLM_GIT_PATH") : "git");

inline const fs::path GCC_PATH = fs::path(std::getenv("CCLM_GCC_PATH") ? std::getenv("CCLM_GCC_PATH") : "gcc");
inline const fs::path GXX_PATH = fs::path(std::getenv("CCLM_GXX_PATH") ? std::getenv("CCLM_GXX_PATH") : "g++");

inline const fs::path AR_PATH  = fs::path(std::getenv("CCLM_AR_PATH")  ? std::getenv("CCLM_AR_PATH")  : "ar");