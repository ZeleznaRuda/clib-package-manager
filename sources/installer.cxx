#include <cstdlib>
#include <iostream>
#include <string>
#include <filesystem>
#include <fstream>
#include <unordered_map>
namespace fs = std::filesystem;
std::unordered_map<std::string, std::string> data = {
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
int main(){
    try {
        const char* home = getenv("HOME");
        fs::path homeDirectory = fs::path(home) / ".clibx";

        fs::path outputDirectory = fs::path(home) / ".local" / "bin";
        fs::path binPath = fs::canonical("/proc/self/exe").parent_path().parent_path() / "bin" / "clibx";
        if (!fs::exists(outputDirectory)){
            fs::create_directories(outputDirectory);
        }
        fs::copy_file(binPath, outputDirectory / "clibx", fs::copy_options::overwrite_existing);
        std::fstream _bashrc (home + std::string("/.bashrc"), std::ios::in | std::ios::out | std::ios::app);
        if (!_bashrc.is_open()){ 
            std::cerr << "\033[31;1;3minstall error\033[0m: cannot open .bashrc file" << std::endl;
            return 1;
        }
        _bashrc.seekg(0, std::ios::beg);
        std::string _bashrcContent((std::istreambuf_iterator<char>(_bashrc)), std::istreambuf_iterator<char>());
        if (_bashrcContent.find("# clibx initialization") == std::string::npos) {
            _bashrc.clear();
            _bashrc << "\n# clibx initialization\n# if you don't know what you're doing, don't change the content !\n";
            _bashrc << "export CLIBX_GIT_PATH=\"git\"\nexport CLIBX_HOME=\"" << homeDirectory.string() << "\"\n";
        }

        fs::permissions((outputDirectory / "clibx"), fs::perms::owner_all | fs::perms::group_read | fs::perms::group_exec | fs::perms::others_read | fs::perms::others_exec, fs::perm_options::add);
        if (fs::create_directories(homeDirectory / "_sys") && fs::create_directories(homeDirectory / "_sys" / "templates") && fs::create_directories(homeDirectory / "_sys" / "libRecords")  ) {
            std::ofstream READMEFile(homeDirectory / "README.md");
            READMEFile << "DO NOT MODIFY THIS FOLDER" << std::endl;
            READMEFile.close();
        for (const auto& [filename, content] : data) {
            std::ofstream file(homeDirectory / "_sys" / "templates" / filename);
            if (!file.is_open()) std::cout << "\033[31;1;3mcannot open file\033[0m" << std::endl;
            file << content;
            file.close();
        }
        } else {}
        std::cout << "\033[32;1minstallation completed (after restarting the device or executing the 'rc' files: clibx will be fully functional ;) )\033[0m" << std::endl;
    } catch (const std::exception& e) {
        std::cerr << "\033[31;1;3minstall error\033[0m: " << e.what() << std::endl;
    }
    return 0;
}