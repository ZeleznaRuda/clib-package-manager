#include <iostream>
#include <unordered_map>
#include <vector>
#include <string>
#include <functional>
#include <iomanip> 
#include <filesystem>
#include <cstdlib>

#include "../include/transactionf.h"
#include "../include/utilsf.h"
#include "../include/parsers.h"
#include "../include/clif.h"
#include "./templates.h"
#include "../include/configf.h"
#include "../include/constants.h"
#include "../include/appf.h"

namespace fs = std::filesystem;

enum class commands{
        APP_INIT,
        APP_CLEAN,
        APP_UNINSTALL,
        APP_CONFIG,
        INSTALL,
        UNINSTALL,
        CONNECT,
        TEMPLATE,
        SEARCH,
        LS,
        INFO,
        UNKNOWN,
};
static const std::unordered_map<std::string, commands> commandMap = {
    {"app-init", commands::APP_INIT},
    {"app-clean", commands::APP_CLEAN},
    {"app-uninstall", commands::APP_UNINSTALL},
    {"app-config", commands::APP_CONFIG},

    {"install", commands::INSTALL},
    {"uninstall", commands::UNINSTALL},
    {"connect", commands::CONNECT},
    {"template", commands::TEMPLATE},
    {"search", commands::SEARCH},
    {"ls", commands::LS},
    {"info", commands::INFO},

};
commands command(std::string& command){
    auto it = commandMap.find(command);
    return (it != commandMap.end()) ? it->second : commands::UNKNOWN;
}
int main(int argc, char* argv[]){
    argvparser::init(argc, argv);
    bool _format = false;
    bool _force = false;
    bool _all = false;
    bool _dep = false;
    std::string _format_user;
    std::string _format_repo;

    argvparser::add_help("app-init",            "initializes clibx");
    argvparser::add_help("install",             "install library                      (supports the '-f' flag && supports the '-d' flag)");
    argvparser::add_help("uninstall",           "uninstall library                    (supports the '-f' flag)");
    argvparser::add_help("connect",             "connect clibx to your project        (supports the '-a' flag)");
    argvparser::add_help("template",            "creates a template based on the name ");
    argvparser::add_help("search",              "checks the repository is available");
    argvparser::add_help("ls",                  "print a list of installed libraries");
    argvparser::add_help("info",                "print package information");
    argvparser::add_help("app-clean",           "deletes the .clibx folder            (supports the '-f' flag)");
    argvparser::add_help("app-uninstall",       "uninstall applications");
    argvparser::add_help("app-config",          "command for configuration management");

    argvparser::define_argument({"--user"}, [](){}, "user flag");
    argvparser::define_argument({"--repo"}, [](){}, "repo flag");
    argvparser::define_argument({"-F", "--format"}, [&_format ,&_format_user ,&_format_repo](){ _format = true;
        if (_format) {
            _format_user = argvparser::get_argument_after({"--user"});
            _format_repo = argvparser::get_argument_after({"--repo"});
        }}, "format flag");

    argvparser::define_argument({"-f", "--force"}, [&_force](){ _force = true;}, "executes the commands without question");
    argvparser::define_argument({"-a", "--all"}, [&_all](){ _all = true;}, "connects all libraries you have installed");
    argvparser::define_argument({"-d", "--dep"}, [&_dep](){ _dep = true;}, "installs dependencies along with the package");
    argvparser::define_argument({"-wh", "--web"}, [](){ system("xdg-open 'https://github.com/rudolfmuller/clibx-package-manager';xdg-open 'https://rudolfmuller.github.io/clibx-package-manager/'"); }, "opens the CLIBX website");
    argvparser::define_argument({"-v", "--version"}, [](){ clif::log(INFO,version); }, "shows the current CLIBX versions");
    argvparser::define_argument({"--git-path"}, [](){}, "setter for git path");
    argvparser::parser();
    
    std::string cmd;
    if (!argvparser::has_argument(1)){
        clif::log(ERROR,"You don't entered a commands. ");
        clif::log(HINT,"use the --help or -h to display the commands list.");
    } else {
        cmd = argvparser::get_argument(1);
    } 

    switch (command(cmd)){
        case commands::APP_INIT:
            appf::app_init();
            break;

        case commands::APP_CLEAN:
            appf::app_clean(_force);
            break;

        case commands::APP_UNINSTALL:
            appf::app_uninstall(_force);
            break;

        case commands::APP_CONFIG: {
            if (argvparser::has_argument(3) && argvparser::has_argument(2) && argvparser::argument({argvparser::get_argument(2)}) == argvparser::argument({"--git-path"})) {
                appf::app_config("git-path", argvparser::get_argument_after({argvparser::argument({argvparser::get_argument(2)})}), _force);
            } else {
                clif::log(FATAL, "incorrect use of the command", -1);
                clif::log(HINT,"use the --help or -h to display the commands list.");
                exit(1);
            }
            break;
        }

        case commands::INSTALL:
            if (_format) {
                transactionf::install(("https://github.com/"+_format_user+"/"+_format_repo+".git"), _force, _dep);
                break;
            }
            transactionf::install(argvparser::get_argument_after({cmd}), _force, _dep);
            break;

        case commands::UNINSTALL:
            transactionf::uninstall(argvparser::get_argument_after({cmd}), _force);
            break;

        case commands::CONNECT:
            transactionf::connect(argvparser::get_argument_after({cmd}), fs::current_path(), _all);
            break;

        case commands::TEMPLATE:
            transactionf::ctemplate(argvparser::get_argument_after({cmd}), fs::current_path());
            break;

        case commands::SEARCH:
            if (_format) {
                transactionf::search(("https://github.com/"+_format_user+"/"+_format_repo+".git"));
                break;
            }
            transactionf::search(argvparser::get_argument_after({cmd}));
            break;

        case commands::LS:
            transactionf::ls();
            break;

        case commands::INFO:
            transactionf::info(argvparser::get_argument_after({cmd}));
            break;

        case commands::UNKNOWN:
             if (!argvparser::is_argument(cmd))clif::log(FATAL, "unknown command: " + cmd);
        default:
            break;
    }



    return 0;
}