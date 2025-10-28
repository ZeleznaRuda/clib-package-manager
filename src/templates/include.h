#pragma once
#include <string>
#include <unordered_map>

namespace templates {

    std::unordered_map<std::string, std::string> data = {
        {"CMakeLists.txt", R"(
cmake_minimum_required(VERSION 3.10)      # Minimum required version of CMake

project(MyProject)                         # Name of the project

# CMake / C++ settings
set(CMAKE_CXX_STANDARD 17)                 # Use C++17 standard
set(CMAKE_CXX_STANDARD_REQUIRED ON)       # Enforce C++17 (no older standard allowed)

# Library definition
add_library(mylib STATIC
    # Source files for the library (.cpp)
    # e.g., libs/mylib/src/mylib.cpp
)

target_include_directories(mylib PUBLIC
    # Include directories for the library (e.g., libs/mylib/include/)
)

# Main program
add_executable(main main.cpp)              # Create an executable from main.cpp

# Link the main program with the library
target_link_libraries(main PRIVATE mylib)

# Include directories used by the main program
target_include_directories(main PRIVATE
    # Include directories (e.g., libs/mylib/include/)
)
)"},

        {"info.yaml", R"(
name: <name>
version: <version>
author: <author>
description: <description>

# Supported operating systems
Windows: true
Linux: true
Darwin: true
)"},
        {"dependencies.txt", R"(
Here you can write a list of dependencies, Git addresses

such as:
https://github.com/ZeleznaRuda/platform-lib.git
https://github.com/ZeleznaRuda/argvparser-engine.git
)"}
    };

} // namespace templates dependencies.txt
