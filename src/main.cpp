#include <iostream>
#include <unordered_map>
#include <vector>
#include <string>
#include <functional>
#include <iomanip> 
#include <filesystem>
#include <cstdlib>

namespace argvparser
{

    namespace {
        std::unordered_map<std::string, std::function<void()>> definedArguments;
        std::unordered_map<std::string, std::string> descriptionsArguments;
        int argc = 0;
        char** argv = nullptr;

        bool starts_with(const std::string& text, const std::string& prefix) {
            return text.rfind(prefix, 0) == 0;
        }
        void help() {
            int maxKeyLength = 0;
            for (const auto& [key, val] : descriptionsArguments) {
                if (key.length() > maxKeyLength)
                    maxKeyLength = key.length();
            }

            for (const auto& [key, val] : descriptionsArguments) {
                std::cout << std::setw(maxKeyLength + 2) << std::left << key
                        << " - " << val << std::endl;
            }
        }
    }
    void add_help(const std::string& name, const std::string& description) {
        descriptionsArguments[name] = description;
    }
    void init(int _argc, char** _argv){
        argc = _argc;
        argv = _argv;

        definedArguments["--help"] = [](){ help(); std::exit(0); };
        definedArguments["-h"] = [](){ help(); std::exit(0); };
        descriptionsArguments["--help"] = "show this help message and exit";
        descriptionsArguments["-h"] = "show this help message and exit";
    }

    int end(){
        return +1;
    }

    template<typename Func>
    void define_argument(const std::vector<std::string>& names, const Func lambda, const std::string description = ""){
        for (const auto& name : names) {
            definedArguments[name] = lambda;
            descriptionsArguments[name] = description;
        }
    }
    
    std::string get_argument(const int index, const std::string defaultValue = "") {
        if (index < 0 || index >= argc){
            return defaultValue;
        }
        return std::string(argv[index]);
    }

    bool has_argument(const int index) {
        return argv[index] != nullptr;
    }

    std::string get_argument_after(const std::vector<std::string>& names, const std::string defaultValue = "") {
        for (int i =0; i < argc; ++i) {
            for (const auto& name : names) {
                if (argv[i] == name && i + 1< argc){
                    return std::string(argv[i+1]);
                }
            }
        }
        return defaultValue;
    }

    int parser(int i = 1) {
        if (argc <= 1) {
            help();
        }
        for (; i < argc; ++i) {
            std::string arg = argv[i];
            if (definedArguments.find(arg) != definedArguments.end()){
                definedArguments[arg]();
            } else {
                if (starts_with(arg, "--")||starts_with(arg, "-")){
                    std::cerr << "\033[1;91munknown argument:\033[0m " << arg << "\n";
                    return 1;
                }
            }
        }
        return 0;
    }

} // namespace argvparser

#include "core/include.h"
#include "templates/include.h"

namespace fs = std::filesystem;

std::string version = "1.0.0";

int main(int argc, char* argv[]){
    argvparser::init(argc, argv);
    bool _force = false;

    argvparser::add_help("init",                "initializes clib");
    argvparser::add_help("install",             "install library                    (supports the '-f' flag)");
    argvparser::add_help("uninstall",           "uninstall library                  (supports the '-f' flag)");
    argvparser::add_help("connect",             "connect clib to your project");
    argvparser::add_help("create",              "creates a template based on the name ('CMakeLists.txt', 'info.yaml')");

    argvparser::define_argument({"-f", "--force"}, [&_force](){ _force = true;}, "executes the command without question");
    argvparser::define_argument({"-v", "--version"}, [](){ core::console::log(version); }, "shows the current CLIB versions");


    argvparser::parser();


    if (!argvparser::has_argument(1)){
        core::console::err("You don't entered a command. \n\t\thint: Use the --help or -h to display the command list.");
    }

    std::string cmd = argvparser::get_argument(1);
    if (cmd == "init"){
        core::init();
    } else if (cmd == "install"){
        core::install(argvparser::get_argument_after({"install"}), _force);
    } else if (cmd == "uninstall"){
        core::uninstall(argvparser::get_argument_after({"uninstall"}), _force);
    } else if (cmd == "connect"){
        core::connect(argvparser::get_argument_after({"connect"}), fs::current_path());
    } else if (cmd == "create"){
        templates::mk(argvparser::get_argument_after({"create"}),fs::current_path());
    }


    return 0;
}