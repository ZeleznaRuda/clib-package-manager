#include <iostream>
#include <unordered_map>
#include <vector>
#include <string>
#include <functional>
#include <iomanip> 
#include <filesystem>
#include <cstdlib>

#include "../include/core.h"
#include "../include/utils.h"
#include "./templates.h"

namespace fs = std::filesystem;

std::string version = "1.1.1";

int main(int argc, char* argv[]){
    argvparser::init(argc, argv);
    bool _force = false;
    bool _all = false;
    bool _Dep = false;



    argvparser::add_help("init",                "initializes clibx");
    argvparser::add_help("install",             "install library                      (supports the '-f' flag && supports the '-d' flag)");
    argvparser::add_help("uninstall",           "uninstall library                    (supports the '-f' flag)");
    argvparser::add_help("connect",             "connect clibx to your project        (supports the '-a' flag)");
    argvparser::add_help("template",            "creates a template based on the name ");
    argvparser::add_help("search",              "checks the repository is available");
    argvparser::add_help("ls",                  "print a list of installed libraries");
    argvparser::add_help("info",                "print package information");
    argvparser::add_help("clean",               "deletes the .clibx folder            (supports the '-f' flag)");


    argvparser::define_argument({"-f", "--force"}, [&_force](){ _force = true;}, "executes the command without question");
    argvparser::define_argument({"-a", "--all"}, [&_all](){ _all = true;}, "connects all libraries you have installed");
    argvparser::define_argument({"-d", "--dep"}, [&_Dep](){ _Dep = true;}, "installs dependencies along with the package");
    argvparser::define_argument({"-w", "--web"}, [](){ system("xdg-open 'https://github.com/rudolfmuller/clibx-package-manager';xdg-open 'https://rudolfmuller.github.io/clibx-package-manager/'"); }, "opens the CLIBX website");



    argvparser::define_argument({"-v", "--version"}, [](){ console::log(version); }, "shows the current CLIBX versions");


    argvparser::parser();

    std::string cmd;
    if (!argvparser::has_argument(1)){
        console::err(1,"You don't entered a command. \n\t\thint: Use the --help or -h to display the command list.");
    } else {
        cmd = argvparser::get_argument(1);
    } 
    
    if (cmd == "init"){
        core::init();
    } else if (cmd == "install"){
            core::install(argvparser::get_argument_after({cmd}), _force, _Dep);
    } else if (cmd == "uninstall"){
            core::uninstall(argvparser::get_argument_after({cmd}), _force);
    } else if (cmd == "connect"){
            core::connect(argvparser::get_argument_after({cmd}), fs::current_path(), _all);
    } else if (cmd == "template"){
            core::ctemplate(argvparser::get_argument_after({cmd}),fs::current_path(),templates::data);
    } else if (cmd == "search"){
            core::search(argvparser::get_argument_after({cmd}));
    } else if (cmd == "ls"){
            core::ls();
    } else if (cmd == "info"){
            core::info(argvparser::get_argument_after({cmd}));
    } else if (cmd == "clean"){
            core::clean(_force);
    }



    return 0;
}