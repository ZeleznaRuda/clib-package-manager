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

namespace console
{
    using namespace std;

    void log(const string& name) {
        cout << "\033[92minfo\033[0m:\n└──────── " << name << endl;
    }

    void warn(const string& name) {
        cout << "\033[93mwarning\033[0m:\n└──────── " << name << endl;
    }

    int err(const string& name) {
        cerr << "\033[91merror\033[0m:\n└──────── " << name << endl;
        return 1;
    }
} // namespace console

namespace utils
{
    fs::path getHomeDirectory() {
#ifdef _WIN32
        const char* home = std::getenv("USERPROFILE");
#else
        const char* home = std::getenv("HOME");
#endif
        return home ? fs::path(home) : fs::path();
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
            return "";
        }

        std::ifstream file(fileName);
        if (!file.is_open()) {
            console::err("cannot open file");
            return "";
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

fs::path homeDirectory = utils::getHomeDirectory() / ".Clib";

int init() {
    try {
        if (fs::create_directory(homeDirectory / "_sys")) {
            console::log("CLIB has been successfully initialized");
            return 0;
        } else {
            return console::err("initialization failed");
        }
    } catch (const fs::filesystem_error& e) {
        return console::err(e.what());
    }
}

int install(const std::string& url, bool force = false) {
    if (!force) {
        char answer;
        std::cout << "Are you sure you want to install the library from '" << url << "'? [Y/n]: ";
        std::cin >> answer;
        answer = std::tolower(static_cast<unsigned char>(answer));
        if (answer != 'y' && answer != '\n') {
            return 0;
        }
    }

    std::string repoName = url.substr(url.find_last_of('/') + 1);
    if (utils::ends_with(repoName, ".git"))
        repoName = repoName.substr(0, repoName.size() - 4);

    fs::path pkgPath = homeDirectory / repoName;

    try {
        if (!fs::create_directory(pkgPath)) {
            return console::err("initialization failed");
        }
    } catch (const fs::filesystem_error& e) {
        return console::err(e.what());
    }

    std::string cmd = "git clone --depth 1 " + url + " " + pkgPath.string();
    int result = system(cmd.c_str());

    if (result != 0) {
        console::err("Git clone failed with code " + std::to_string(result));
    } else {
        console::log("Library installed successfully to " + pkgPath.string());
    }

    auto infoData = yaml::parser(yaml::reed(pkgPath / "info.yaml"));

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
        pkgFile << "# - SOURCES - " << std::endl;
        pkgFile << "git-url: " << url << std::endl;
        pkgFile << "installation-date: " << std::put_time(now, "%d.%m.%Y-%H:%M:%S") << std::endl;
        pkgFile.close();

        console::log("record successfully created " + (homeDirectory / "_sys" / (infoData["name"] + "-package.yaml")).string());
    }

    return 0;
}

int main() {
    return install("https://github.com/ZeleznaRuda/platform-lib.git");
}
