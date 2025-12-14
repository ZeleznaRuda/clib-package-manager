#include <cstdlib>
#include <iostream>
#include <string>
#include <filesystem>
#include <fstream>
#include <unordered_map>
#include <unistd.h>
namespace fs = std::filesystem;
std::unordered_map<std::string, std::string> data = {
    {"CMakeLists.txt", R"(cmake_minimum_required(VERSION 3.10)

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

    {"package.yml", R"(name: 
description: 
version: 

authors: [...]
license: MIT
websites: [...]


build-mode: # shared | static 
build-compiler: # g++ | gcc
build-cflags: [...]
build-dependencies: [...]
build-include-directory: 
build-source-files: [...]
)"},{"run.sh", R"(#!/bin/bash
set -e

# setup:
COMPILLER="g++"             # < here you choose which compiler you want to use.
INPUT="main.cpp"            # < here you choose which file you want to compile.
OUTPUT="main"               # < here you choose which file you want to out.
PKG="foo-lib@x.y.z"         # < here you choose which package you want to use 
LIB="foo"                   # < here you choose which libary you want to use (.so OR .a)

$COMPILLER $INPUT -I$HOME/.cclm/$PKG/include/ \
    -L$HOME/.cclm/$PKG -l$LIB \
    -Wl,-rpath,$HOME/.cclm/$PKG -o $OUTPUT \
    && ./$OUTPUT

)"},
};
int main(){
    if (geteuid() == 0) {
        std::cerr << "\033[31;1;3minstall error\033[0m: don't run the installer as administrator (root)." << std::endl;
        return 1;
    }
    try {
        const char* home = getenv("HOME");
        fs::path homeDirectory = fs::path(home) / ".cclm";

        fs::path outputDirectory = fs::path(home) / ".local" / "bin";
        fs::path binPath = fs::canonical("/proc/self/exe").parent_path().parent_path() / "bin" / "cclm";
        if (!fs::exists(outputDirectory)){
            fs::create_directories(outputDirectory);
        }
        fs::copy_file(binPath, outputDirectory / "cclm", fs::copy_options::overwrite_existing);

        fs::permissions((outputDirectory / "cclm"),
                        fs::perms::owner_all |
                        fs::perms::group_read | fs::perms::group_exec |
                        fs::perms::others_read | fs::perms::others_exec,
                        fs::perm_options::add);

        if (fs::create_directories(homeDirectory / "_sys") &&
            fs::create_directories(homeDirectory / "_sys" / "templates") &&
            fs::create_directories(homeDirectory / "_sys" / "logs") &&
            fs::create_directories(homeDirectory / "_sys" / "registry")) {

            std::ofstream READMEFile(homeDirectory / "README.md");
            READMEFile << "DO NOT MODIFY THIS FOLDER" << std::endl;
            READMEFile.close();

            for (const auto& [filename, content] : data) {
                std::ofstream file(homeDirectory / "_sys" / "templates" / filename);
                if (!file.is_open()) std::cout << "\033[31;1;3mcannot open file\033[0m" << std::endl;
                file << content;
                file.close();
            }
        }

        std::cout << "\033[32;1minstallation completed (cclm: hello world)\033[0m" << std::endl;
    } catch (const std::exception& e) {
        std::cerr << "\033[31;1;3minstall error\033[0m: " << e.what() << std::endl;
    }

    return 0;
}