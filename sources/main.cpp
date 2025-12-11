#include <iostream>
#include <unordered_map>
#include <vector>
#include <string>
#include <functional>
#include <iomanip> 
#include <filesystem>
#include <cstdlib>

#include "../include/argvparserf.h"
#include "../include/constants.h"
#include "../include/clif.h"
#include "../include/transactionf.h"
#include "../include/appf.h"

namespace fs = std::filesystem;

enum class commands{
        PURGE,
        INSTALL,
        REMOVE,
        CONNECT,
        TEMPLATE,
        SEARCH,
        EXIST,
        LS,
        INFO,
        GIT,
        REPORT,
        UNKNOWN
};
static const std::unordered_map<std::string, commands> commandMap = {
    {"purge", commands::PURGE},
    {"install", commands::INSTALL},
    {"remove", commands::REMOVE},
    {"connect", commands::CONNECT},
    {"template", commands::TEMPLATE},
    {"search", commands::SEARCH},
    {"ls", commands::LS},
    {"info", commands::INFO},
    {"git", commands::GIT},
    {"report", commands::REPORT},
    {"exist", commands::EXIST}

};
commands command(std::string& command){
    auto it = commandMap.find(command);
    return (it != commandMap.end()) ? it->second : commands::UNKNOWN;
}
std::string repo(const std::string& user, const std::string& repo){
    return ("https://github.com/"+user+"/"+repo+".git");
}
int main(int argc, char* argv[]){
    argvparser::init(argc, argv);
    bool _force = false;
    bool _all = false;
    bool _dep = false;
    bool _url = false;

    

    argvparser::add_help("install",             "install library                      (supports the '-f' flag && supports the '-d' flag)");
    argvparser::add_help("remove",              "remove library                       (supports the '-f' flag)");
    argvparser::add_help("connect",             "connect clibx to your project        (supports the '-a' flag)");
    argvparser::add_help("template",            "creates a template based on the name ");
    argvparser::add_help("search",              "checks the repository is available");
    argvparser::add_help("exist",               "checks if the library is installed");
    argvparser::add_help("ls",                  "print a list of installed libraries");
    argvparser::add_help("info",                "print package information");
    argvparser::add_help("git",                 "git command wrapper (for debugging purposes)");
    argvparser::add_help("report",              "report a bug or suggest a new feature");

    argvparser::add_help("purge",               "uninstalls the application and removes all its traces.");

    argvparser::define_argument({"-u", "--url"}, [&_url](){ _url = true;}, "takes the raw url address instead of the author and package name");
    argvparser::define_argument({"-f", "--force"}, [&_force](){ _force = true;}, "executes the commands without question");
    argvparser::define_argument({"-a", "--all"}, [&_all](){ _all = true;}, "connects all libraries you have installed");
    argvparser::define_argument({"-W", "--web"}, [](){ sysf({"xdg-open", "'https://github.com/rudolfmuller/clibx-package-manager' ;","xdg-open", "'https://rudolfmuller.github.io/clibx-package-manager/'"}); }, "opens the CLIBX website");
    argvparser::define_argument({"-v", "--version"}, [](){ clif::log(INFO,std::string(VERSION)); }, "shows the current CLIBX versions");
    argvparser::parser();
    
    std::string cmd;
    if (!argvparser::has_argument(1)){
        clif::log(ERROR,"You don't entered a commands. ");
        clif::log(HINT,"use the --help or -h to display the commands list.");
        return 1;
    }
    cmd = argvparser::get_argument(1);
    

    switch (command(cmd)) {
        case commands::PURGE:
            appf::purge(_force);
            break;


        case commands::INSTALL:
            if (!_url && argvparser::has_argument(3)){
                transactionf::install(repo(argvparser::get_argument(2), argvparser::get_argument(3)), _force, _dep);
                break;
            }
            transactionf::install(argvparser::get_argument_after({cmd}), _force, _dep);
            break;

        case commands::REMOVE:
            transactionf::remove(argvparser::get_argument_after({cmd}), _force);
            break;

        case commands::CONNECT:
            transactionf::connect(argvparser::get_argument_after({cmd}), fs::current_path(), _all);
            break;

        case commands::TEMPLATE:
            transactionf::use_template(argvparser::get_argument_after({cmd}), fs::current_path());
            break;

        case commands::SEARCH:
            transactionf::search(repo(argvparser::get_argument(2), argvparser::get_argument(3)));
            break;
        case commands::EXIST:
            transactionf::exist(argvparser::get_argument_after({cmd}));
            break;
        case commands::LS:
            transactionf::ls();
            break;

        case commands::INFO:
            transactionf::info(argvparser::get_argument_after({cmd}));
            break;

        case commands::GIT:
            transactionf::git(argvparser::get_argument_after({cmd}));
            break;
        case commands::REPORT:
            appf::report(argvparser::get_argument(2), argvparser::get_argument(3),argvparser::get_argument(4), argvparser::get_argument(5));
            break;
        case commands::UNKNOWN:
            if (!argvparser::is_argument(cmd))clif::log(FATAL, "unknown command: " + cmd);
        default:
            break;
    }



    return 0;
}