#pragma once
#include <string>
#include <unordered_map>

namespace templates {

inline std::unordered_map<std::string, std::string> data = {
    {"CMakeLists.txt", R"(
cmake_minimum_required(VERSION 3.10)

project(MyProject)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

add_library(mylib STATIC
)

target_include_directories(mylib PUBLIC
)

add_executable(main main.cpp)

target_link_libraries(main PRIVATE mylib)

target_include_directories(main PRIVATE
)
)"},

    {"info.yaml", R"(
name: <name>
version: <version>
author: <author>
description: <description>

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

} // namespace templates
