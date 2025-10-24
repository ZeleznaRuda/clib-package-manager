#include <iostream>
#include <cstdlib>
#include <string>
#include <filesystem>

namespace fs = std::filesystem;

namespace console
{
    using namespace std;
    void log(string name){
            cout << "\033[92minfo\033[0m:\n└──────── "<<name<< std::endl;
    }  
    void warn(string name){
            cout << "\033[93mwarning\033[0m:\n└──────── "<<name<< std::endl;
    }  
    int err(string name){
            cerr << "\033[91merror\033[0m:\n└──────── "<<name<< std::endl;
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
} // namespace utils



fs::path homeDirectory = utils::getHomeDirectory() / ".Clib";

int init(){
    try{
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

int install(std::string url, bool force=false){
    if (!force) {
        char answer;
        std::cout << "Are you sure you want to install the library from '" << url << "'? [Y/n]: ";
        std::cin >> answer;
        answer = std::tolower(static_cast<unsigned char>(answer));
        if (answer != 'y' && answer != '\n'){
            return 0;
        }
    }
    
    std::string repoName = url.substr(url.find_last_of('/') + 1);
    if (utils::ends_with(repoName, ".git"))
        repoName = repoName.substr(0, repoName.size() - 4);

    fs::path pkgPath = homeDirectory / repoName;    

    try{
        if (!fs::create_directory(pkgPath)) {
            return console::err("initialization failed");
        }
    } catch (const fs::filesystem_error& e) {
        return console::err(e.what());
    }

    std::string cmd = std::string("git clone --depth 1 ") + url + " " + pkgPath.string();
    int result = system(cmd.c_str());
    if (result != 0) {
        console::err("Git clone failed with code " + std::to_string(result));
    } else {
        console::log("Library installed successfully to " + pkgPath.string());
    }
    return 0;
}

int main(){
    return install("https://github.com/ZeleznaRuda/platform-lib.git");
}