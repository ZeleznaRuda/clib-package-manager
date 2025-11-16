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
#include "../include/parsers.h"
#include "../include/cli.h"
#include "./templates.h"
#include "../include/configf.h"

namespace fs = std::filesystem;

std::string version = "1.1.4";

int main(int argc, char* argv[]){
    argvparser::init(argc, argv);
    bool _force = false;
    bool _all = false;
    bool _Dep = false;
    std::string cmd;

    if (!argvparser::has_argument(1)){
        cli::log(ERROR,"You don't entered a command. ");
        cli::log(HINT,"use the --help or -h to display the command list.");
    } else {
        cmd = argvparser::get_argument(1);
    } 


    argvparser::add_help("apk-init",            "initializes clibx");
    argvparser::add_help("install",             "install library                      (supports the '-f' flag && supports the '-d' flag)");
    argvparser::add_help("uninstall",           "uninstall library                    (supports the '-f' flag)");
    argvparser::add_help("connect",             "connect clibx to your project        (supports the '-a' flag)");
    argvparser::add_help("template",            "creates a template based on the name ");
    argvparser::add_help("search",              "checks the repository is available");
    argvparser::add_help("ls",                  "print a list of installed libraries");
    argvparser::add_help("info",                "print package information");
    argvparser::add_help("apk-clean",           "deletes the .clibx folder            (supports the '-f' flag)");
    argvparser::add_help("apk-uninstall",       "uninstall applications");


    argvparser::define_argument({"-f", "--force"}, [&_force](){ _force = true;}, "executes the command without question");
    argvparser::define_argument({"-a", "--all"}, [&_all](){ _all = true;}, "connects all libraries you have installed");
    argvparser::define_argument({"-d", "--dep"}, [&_Dep](){ _Dep = true;}, "installs dependencies along with the package");
    argvparser::define_argument({"-wh", "--web"}, [](){ system("xdg-open 'https://github.com/rudolfmuller/clibx-package-manager';xdg-open 'https://rudolfmuller.github.io/clibx-package-manager/'"); }, "opens the CLIBX website");
    
    argvparser::define_argument({"--git-path"}, [](){}, "setter for git path");


    argvparser::define_argument({"-v", "--version"}, [](){ cli::log(INFO,version); }, "shows the current CLIBX versions");


    argvparser::parser();
    
    if (cmd == "apk-init"){
        core::apk_init();
    } else if (cmd == "apk-clean"){
            core::apk_clean(_force);
    } else if (cmd == "apk-uninstall"){
            core::apk_uninstall(_force);
    } else if (cmd == "apk-config"){
           if (argvparser::get_argument_after({cmd}) == argvparser::argument({"-gp", "--git-path"})) {
                configf::changeConfig("git-path", argvparser::get_argument_after({argvparser::argument({"-gp", "--git-path"})}));
           }
    } else if (cmd == "install"){
            core::install(argvparser::get_argument_after({cmd}), _force, _Dep);
    } else if (cmd == "uninstall"){
            core::uninstall(argvparser::get_argument_after({cmd}), _force);
    } else if (cmd == "connect"){
            core::connect(argvparser::get_argument_after({cmd}), fs::current_path(), _all);
    } else if (cmd == "template"){
            core::ctemplate(argvparser::get_argument_after({cmd}),fs::current_path());
    } else if (cmd == "search"){
            core::search(argvparser::get_argument_after({cmd}));
    } else if (cmd == "ls"){
            core::ls();
    } else if (cmd == "info"){
            core::info(argvparser::get_argument_after({cmd}));
    } 

    return 0;
}