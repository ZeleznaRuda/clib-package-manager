#include <iostream>
#include <cstdlib>
#include <string>
#include <filesystem>
#include <vector>
#include <sstream>
#include <fstream>
#include <unordered_map>
#include <ctime>
#include <iomanip>
#include <regex>

#include "../include/core.h"
#include "../include/utils.h"

namespace fs = std::filesystem;

namespace core {

fs::path homeDirectory = utils::getHomeDirectory() / ".clibx";

void init() {
    try {
        if (fs::create_directory(homeDirectory)) {
            std::ofstream READMEFile(homeDirectory / "README.md");
            READMEFile << "DO NOT MODIFY THIS FOLDER" << std::endl;
            READMEFile.close();
            console::log("CLIBX has been successfully initialized");
        } else {
            console::err(1,"initialization failed");
        }
    } catch (const fs::filesystem_error& e) {
        console::err(1,e.what());
    }
}

void install(const std::string& url, const bool force, const bool installDependencies) {
    if (!force) {
        std::cout << "Are you sure you want to install the library from '" << url << "'? [Y/n]: ";
        std::string input;
        std::getline(std::cin, input);
        char answer = input.empty() ? 'n' : std::tolower(input[0]);
        if (answer != 'y') {
            console::log("installation cancelled by user");
            return;
        }
    }

    if (!(utils::start_with(url, "http") || utils::start_with(url, "git@")) 
    || !utils::ends_with(url, ".git")) {
        console::err(1,"invalid git url format");
    }
    std::string repoName = url.substr(url.find_last_of('/') + 1);
    if (utils::ends_with(repoName, ".git"))
        repoName = repoName.substr(0, repoName.size() - 4);

    fs::path pkgPath = homeDirectory / repoName;

    try {
        if (!fs::create_directory(pkgPath)) {
            console::err(2,"initialization failed");
        }
    } catch (const fs::filesystem_error& e) {
        console::err(2,e.what());
    }

    std::string command = "git clone --depth 1 " + utils::escapeShellArg(url) + " " + utils::escapeShellArg(pkgPath.string()) + " &> /dev/null";
    int result=system(command.c_str());

    auto infoData = yaml::parser(yaml::read(pkgPath / "info.yaml"));

    if (result != 0) {
        console::err(result,"git clone failed with code " + std::to_string(result));
    } else {
        try{
            fs::path newPath = homeDirectory / infoData["name"];
            if (fs::exists(newPath)){
                fs::remove_all(pkgPath);
                console::err(1, "the library is already installed.");
            }
            fs::rename(pkgPath.string(), newPath);
            pkgPath = newPath;

        }catch (fs::filesystem_error& e) {
            console::err(2,std::string("error: ") + e.what() + "\n");
        }
        console::log("library installed successfully to " + pkgPath.string());
    }

    fs::create_directories(homeDirectory / "_sys");

    std::ofstream pkgFile(homeDirectory / "_sys" / (infoData["name"] + "-package.yaml"));
    if (!pkgFile) {
        console::err(2,"failed to create record: " + (homeDirectory / "_sys" / (infoData["name"] + "-package.yaml")).string());
    } else {
        std::time_t t = std::time(nullptr);
        std::tm* now = std::localtime(&t);
        pkgFile << "# package information:" << std::endl;
        pkgFile << "name: " << infoData["name"] << std::endl;
        pkgFile << "description: " << infoData["description"] << std::endl;
        pkgFile << "version: " << infoData["version"] << std::endl;

        pkgFile << std::endl;
        pkgFile << "# origin:" << std::endl;
        pkgFile << "author: " << infoData["author"] << std::endl;
        pkgFile << "git-url: " << url << std::endl;
        pkgFile << "installation-date: " << std::put_time(now, "%d.%m.%Y-%H:%M:%S") << std::endl;

        console::log("record successfully created " + (homeDirectory / "_sys" / (infoData["name"] + "-package.yaml")).string());
    }
    if (installDependencies && fs::exists(pkgPath / "dependencies.txt")){
        auto dependenciesData = lister::parser(yaml::read(pkgPath / "dependencies.txt"));
        pkgFile << std::endl;
        pkgFile << "# dependencies:" << std::endl;
        for (const auto& dep : dependenciesData){
            console::log("installing dependency " + dep);
            core::install(dep, true, false);
            pkgFile << "dependence: " << dep << std::endl;
        }
    }
    pkgFile.close();
}

void uninstall(const std::string& pkgName, bool force) {
    if (!force) {
        std::cout << "Are you sure you want to remove '" << pkgName << "' library? [Y/n]: ";
        std::string input;
        std::getline(std::cin, input);
        char answer = input.empty() ? 'n' : std::tolower(input[0]);
        if (answer != 'y') {
            console::log("uninstallation cancelled by user");
            return;
        }
    }

    fs::path pkgPath = homeDirectory / pkgName;
    fs::path pkgInfoFilePath = homeDirectory / "_sys" / (pkgName + "-package.yaml");

    if (!fs::exists(pkgPath)) {
        console::err(1, "library '" + pkgName + "' does not exist.");
    }

    try {
        if (fs::exists(pkgPath)) fs::remove_all(pkgPath);
        if (fs::exists(pkgInfoFilePath)) fs::remove(pkgInfoFilePath);
        console::log("library '" + pkgName + "' removed successfully.");
    } catch (const fs::filesystem_error& e) {
        console::err(2, "error removing library: " + std::string(e.what()));
    }
}

void connect(const std::string& pkgName, const fs::path& targetDirectory, const bool all) {
    std::vector<fs::path> packages;

    if (all) {
        for (auto& entry : fs::directory_iterator(homeDirectory)) {
            if (fs::is_directory(entry.path())) {
                std::string name = entry.path().filename().string();
                if (name.empty() || name[0] == '_') {
                    continue; 
                }
                packages.push_back(entry.path());
            }
        }
    } else {
        fs::path pkgPath = homeDirectory / pkgName;
        if (!fs::exists(pkgPath)) {
            console::err(1,"library " + pkgName +" not found\n");
        }
        packages.push_back(pkgPath);
    }

    for (auto& pkgPath : packages) {
        std::string name = pkgPath.filename().string();
        fs::path projectDirectory = targetDirectory / "libs" / name;

        fs::create_directories(projectDirectory.parent_path());

        if (fs::exists(projectDirectory) || fs::is_symlink(projectDirectory)) {
            std::error_code ec_remove;
            if (fs::remove(projectDirectory, ec_remove)) {
                console::log("disconnecting the project: " + projectDirectory.string() + "\n");
            } else {
                console::err(2,"error disconnecting the project: "+ ec_remove.message()+"\n");
            }
        }

        std::error_code ec;
        fs::create_directory_symlink(pkgPath, projectDirectory, ec);
        if (ec) {
            console::err(1,"error occurred when connecting project: " + ec.message() +"\n");
        } else {
            console::log("package '"+ projectDirectory.string() +"' was successfully connected\n");
        }
    }
}

void ctemplate(const std::string& name, const std::filesystem::path& targetDirectory, std::unordered_map<std::string, std::string>& data){

    if (data.find(name) != data.end()){
            std::ofstream templateFile(targetDirectory / (name));
            if (!templateFile) {
                console::err(2,"failed to create template: " + targetDirectory.string());
            }
            templateFile << data[name];
            templateFile.close();

            console::log("template successfully created " + targetDirectory.string());
            
        } else {
            for (const auto& [key, value] : data) {
                std::cout << "---" << key << "---" << value << "\n";}
            console::err(1, "template '" + name + "' does not exist");

        }
    }

void search(const std::string& repoName){
    std::string command = "git ls-remote "+ utils::escapeShellArg(repoName)+" &> /dev/null";
    int result = system(command.c_str());
    if (result == 0){
        console::log("the library is accessible");
    } else {
        console::warn("library is not accessible");
    }
}
void info(const std::string& repoName){
    const std::string suffix = "-package.yaml";

    std::string name = repoName;

    if (name.size() >= suffix.size() &&
            name.compare(name.size() - suffix.size(), suffix.size(), suffix) == 0) {
            name.erase(name.size() - suffix.size());
    }
    std::ifstream file(homeDirectory / "_sys" / (name + suffix)); 
    if (!file.is_open()) {
        console::err(1, "package does not exist");
    }

    std::string line;
    while (std::getline(file, line)) {  
        std::cout << line << std::endl;
    }

    file.close();  
}
void ls(){
    std::cout << "Installed library:" << std::endl;

    for (const auto& entry : fs::directory_iterator(homeDirectory)) {
        if (!fs::is_directory(entry.path())) continue;
        std::string name = entry.path().filename().string();
        if (name == "_sys") continue;
        std::cout << "\t" << name << std::endl;
    }
}
}
/*    for (const auto& entry : fs::directory_iterator(path))
        std::cout << entry.path() << std::endl;*/