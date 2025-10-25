#include "include.h"
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

namespace fs = std::filesystem;
namespace core {

namespace console
{
    using namespace std;

    void log(const string& name) {
        cout << "\033[92minfo\033[0m:\n└──────── " << name << endl;
    }

    void warn(const string& name) {
        cout << "\033[93mwarning\033[0m:\n└──────── " << name << endl;
    }

    void err(const string& name) {
        cerr << "\033[91merror\033[0m:\n└──────── " << name << endl;
        exit(1);
    }
} // namespace console

namespace utils
{
    fs::path getHomeDirectory() {
        const char* home = std::getenv("HOME");
        return home;
    }

    bool ends_with(const std::string& str, const std::string& suffix) {
        return str.size() >= suffix.size() &&
               str.compare(str.size() - suffix.size(), suffix.size(), suffix) == 0;
    }

    std::vector<std::string> split(const std::string& text, char delimiter) {
        std::vector<std::string> result;
        std::stringstream ss(text);
        std::string item;

        while (std::getline(ss, item, delimiter)) {
            result.push_back(item);
        }

        return result;
    }

    std::string strip(const std::string& s) {
        size_t start = 0;
        size_t end = s.size();

        while (start < s.size() && std::isspace(static_cast<unsigned char>(s[start])))
            ++start;

        while (end > start && std::isspace(static_cast<unsigned char>(s[end - 1])))
            --end;

        return s.substr(start, end - start);
    }
} // namespace utils

namespace yaml
{
    std::string reed(const fs::path& fileName) {
        if (!fs::exists(fileName)) {
            console::err("file " + fileName.string() + " not found");
        }

        std::ifstream file(fileName);
        if (!file.is_open()) {
            console::err("cannot open file");
        }

        std::string fileContent((std::istreambuf_iterator<char>(file)),
                                 std::istreambuf_iterator<char>());

        return fileContent;
    }

    std::unordered_map<std::string, std::string> parser(const std::string& fileContent) {
        std::unordered_map<std::string, std::string> data;
        auto lines = utils::split(fileContent, '\n');

        for (auto& l : lines) {
            l = utils::strip(l);
            if (l.empty() || l[0] == '#') continue;
            auto parts = utils::split(l, ':');
            if (parts.size() == 2) {
                std::string key = utils::strip(parts[0]);
                std::string value = utils::strip(parts[1]);
                data[key] = value;
            }
        }
        return data;
    }
} // namespace yaml

fs::path homeDirectory = utils::getHomeDirectory() / ".clib";

void init() {
    try {
        if (fs::create_directory(homeDirectory)) {
            console::log("CLIB has been successfully initialized");
        } else {
            console::err("initialization failed");
        }
    } catch (const fs::filesystem_error& e) {
        console::err(e.what());
    }
}

void install(const std::string& url, const bool force) {
    if (!force) {
        char answer;
        std::cout << "Are you sure you want to install the library from '" << url << "'? [Y/n]: ";
        std::cin >> answer;
        answer = std::tolower(static_cast<unsigned char>(answer));
        if (answer != 'y') {
            return;
        }
    }

    std::string repoName = url.substr(url.find_last_of('/') + 1);
    if (utils::ends_with(repoName, ".git"))
        repoName = repoName.substr(0, repoName.size() - 4);

    fs::path pkgPath = homeDirectory / repoName;

    try {
        if (!fs::create_directory(pkgPath)) {
            console::err("initialization failed");
        }
    } catch (const fs::filesystem_error& e) {
        console::err(e.what());
    }

    std::string cmd = "git clone --depth 1 " + url + " " + pkgPath.string();
    int result = system(cmd.c_str());
    auto infoData = yaml::parser(yaml::reed(pkgPath / "info.yaml"));
    if (result != 0) {
        console::err("git clone failed with code " + std::to_string(result));
    } else {
        try{
            fs::rename(pkgPath.string(), homeDirectory / infoData["name"]);
        }catch (fs::filesystem_error& e) {
            console::err(std::string("error: ") + e.what() + "\n");
        }
        console::log("library installed successfully to " + pkgPath.string());
    }

    fs::create_directories(homeDirectory / "_sys");

    std::ofstream pkgFile(homeDirectory / "_sys" / (infoData["name"] + "-package.yaml"));
    if (!pkgFile) {
        console::err("failed to create record: " + (homeDirectory / "_sys" / (infoData["name"] + "-package.yaml")).string());
    } else {
        std::time_t t = std::time(nullptr);
        std::tm* now = std::localtime(&t);

        for (const auto& [key, value] : infoData) {
            pkgFile << key << ": " << value << "\n";
        }
        pkgFile << "" << std::endl;
        pkgFile << "git-url: " << url << std::endl;
        pkgFile << "installation-date: " << std::put_time(now, "%d.%m.%Y-%H:%M:%S") << std::endl;
        pkgFile.close();

        console::log("record successfully created " + (homeDirectory / "_sys" / (infoData["name"] + "-package.yaml")).string());
    }
}

void uninstall(const std::string& pkgName, bool force) {
    if (!force) {
        char answer;
        std::cout << "Are you sure you want to remove '" << pkgName << "' library? [Y/n]: ";
        std::cin >> answer;
        answer = std::tolower(static_cast<unsigned char>(answer));
        if (answer != 'y') {
            return;
        }
    }
    fs::path pkgPath = homeDirectory / pkgName;

    auto infoData = yaml::parser(yaml::reed(pkgPath / "info.yaml"));

    fs::path pkgInfoFilePath = homeDirectory / "_sys" / (infoData["name"] + "-package.yaml");

    try {
        if (fs::exists(pkgPath) && fs::exists(pkgInfoFilePath) ) {
            fs::remove_all(pkgPath);
            fs::remove(pkgInfoFilePath);
            console::log("library '" + pkgName + "' removed successfully.\n");
        } else {
            console::err("library '" + pkgName + "' does not exist.\n");
        }
    } catch (const fs::filesystem_error& e) {
        console::err(std::string("error removing library: ") + e.what() + "\n");
    }
}


void connect(const std::string& pkgName, const fs::path& targetDirectory) {
    // ai//my
    fs::path pkgPath = homeDirectory / pkgName;
    if (!fs::exists(pkgPath)) {
        console::err("library " + pkgName +" not found\n");
    }

    fs::path projectDirectory = targetDirectory / "libs" / pkgName;

    fs::create_directories(projectDirectory.parent_path());

    if (fs::exists(projectDirectory) || fs::is_symlink(projectDirectory)) {
        std::error_code ec_remove;
        if (fs::remove(projectDirectory, ec_remove)) {
            console::log("disconnecting the project: "+projectDirectory.string() +"\n");
        } else {
            console::err("error disconnecting the project: "+ ec_remove.message()+"\n");
        }
    }

    std::error_code ec;
    fs::create_directory_symlink(pkgPath, projectDirectory, ec);
    if (ec) {
        console::err("error occurred when connecting project: " + ec.message() +"\n");
    } else {
        console::log("package '"+std::string(projectDirectory)+"' was successfully connected"+"\n");
    }
}
}

// .Clib filesystem error