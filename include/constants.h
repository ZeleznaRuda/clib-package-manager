#pragma once
#include <filesystem>
#include <string>
#include <cstdlib>

namespace fs = std::filesystem;

inline constexpr std::string_view VERSION = "2.1.4";
inline const     fs::path HOME = fs::path(getenv("HOME"));
inline const     fs::path HOME_DIRECTORY = HOME / ".clibx";
