#include <iostream>
#include <unordered_map>
#include <vector>
#include <string>
#include <functional>
#include <iomanip> 
#include <filesystem>
#include <cstdlib>
#include "core/include.h"
#include "templates/include.h"
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

    void parser(int i = 1) {
        if (argc <= 1) {
            help();
        }
        for (; i < argc; ++i) {
            std::string arg = argv[i];
            if (definedArguments.find(arg) != definedArguments.end()){
                definedArguments[arg]();
            } else {
                if (starts_with(arg, "--")||starts_with(arg, "-")){
                    core::console::err(1,"\033[1;91munknown argument:\033[0m " + arg + "\n");
                }
            }
        }
    }

} // namespace argvparser



namespace fs = std::filesystem;

std::string version = "1.0.2";

int main(int argc, char* argv[]){
    argvparser::init(argc, argv);
    bool _force = false;
    bool _all = false;
    bool _installDep = false;



    argvparser::add_help("init",                "initializes clibx");
    argvparser::add_help("install",             "install library                      (supports the '-f' flag && supports the '-I' flag)");
    argvparser::add_help("uninstall",           "uninstall library                    (supports the '-f' flag)");
    argvparser::add_help("connect",             "connect clibx to your project        (supports the '-a' flag)");
    argvparser::add_help("template",            "creates a template based on the name ");
    argvparser::add_help("search",              "checks the repository is available");

    argvparser::define_argument({"-f", "--force"}, [&_force](){ _force = true;}, "executes the command without question");
    argvparser::define_argument({"-a", "--all"}, [&_all](){ _all = true;}, "connects all libraries you have installed");
    argvparser::define_argument({"-I", "--instDep"}, [&_installDep](){ _installDep = true;}, "installs dependencies along with the package");
    argvparser::define_argument({"-w", "--web"}, [](){ system("xdg-open 'https://zeleznaruda.github.io/clibx-package-manager/'"); }, "opens the CLIBX website");



    argvparser::define_argument({"-v", "--version"}, [](){ core::console::log(version); }, "shows the current CLIBX versions");


    argvparser::parser();


    if (!argvparser::has_argument(1)){
        core::console::err(1,"You don't entered a command. \n\t\thint: Use the --help or -h to display the command list.");
    }

    std::string cmd = argvparser::get_argument(1);
    if (cmd == "init"){
        core::init();
    } else if (cmd == "install"){
        if (argvparser::has_argument(2) ){
            core::install(argvparser::get_argument_after({"install"}), _force, _installDep);
        } else {
            core::console::err(1, "we expect an argument");
        }
    } else if (cmd == "uninstall"){
        if (argvparser::has_argument(2)){
            core::uninstall(argvparser::get_argument_after({"uninstall"}), _force);
        } else {
            core::console::err(1, "we expect an argument");
        }
    } else if (cmd == "connect"){
        if (argvparser::has_argument(2)){
            core::connect(argvparser::get_argument_after({"connect"}), fs::current_path(), _all);
        } else {
            core::console::err(1, "we expect an argument");
        }
    } else if (cmd == "template"){
        if (argvparser::has_argument(2)){
            core::ctemplate(argvparser::get_argument_after({"template"}),fs::current_path(),templates::data);
        } else {
            core::console::err(1, "we expect an argument");
        }
    } else if (cmd == "search"){
        if (argvparser::has_argument(2)){
            core::search(argvparser::get_argument_after({"search"}));
        } else {
            core::console::err(1, "we expect an argument");
        }
    }


    return 0;
}
