#include <iostream>
#include <unordered_map>
#include <vector>
#include <string>
#include <functional>
#include <iomanip> 
#include <filesystem>
#include <cstdlib>

#include "../include/argvf.h"
#include "../include/constants.h"
#include "../include/clif.h"
#include "../include/transactionf.h"
#include "../include/appf.h"

namespace fs = std::filesystem;
using namespace transactionf;
using namespace argvparser;

enum class commands{
        PURGE,
        INSTALL,
        REMOVE,
        RUN,
        TEMPLATE,
        SEARCH,
        EXIST,
        LS,
        INFO,
        GIT,
        WEBSITE,
        REPORT,
        UNKNOWN
};
static const std::unordered_map<std::string, commands> commandMap = {
    {"purge", commands::PURGE},
    {"install", commands::INSTALL},
    {"run", commands::RUN},
    {"remove", commands::REMOVE},
    {"template", commands::TEMPLATE},
    {"search", commands::SEARCH},
    {"ls", commands::LS},
    {"info", commands::INFO},
    {"git", commands::GIT},
    {"report", commands::REPORT},
    {"website", commands::WEBSITE},
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
    bool _url = false;

    

    add_help("install",             "install library\t\t\t(supports the '-f' flag)");
    add_help("remove",              "remove library\t\t\t(supports the '-f' flag)");
    add_help("run",                 "compile and run your project");
    add_help("template",            "creates a template based on the name ");
    add_help("search",              "checks the repository is available");
    add_help("exist",               "checks if the library is installed");
    add_help("ls",                  "print a list of installed libraries");
    add_help("info",                "print package information");
    add_help("git",                 "git command wrapper (for debugging purposes)");
    add_help("report",              "report a library");
    add_help("website",             "open websites of the library");


    add_help("purge",               "uninstalls the application and removes all its traces.");

    define_argument({"-u", "--url"}, [&_url](){ _url = true;}, "takes the raw url address instead of the author and package name");
    define_argument({"-f", "--force"}, [&_force](){ _force = true;}, "executes the commands without question");
    define_argument({"-a", "--all"}, [&_all](){ _all = true;}, "connects all libraries you have installed");
    define_argument({"-v", "--version"}, [](){ clif::log(INFO,std::string(VERSION)); }, "shows the current CCLM versions");
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
                install(repo(argvparser::get_argument(2), argvparser::get_argument(3)), _force);
                break;
            }
            install(argvparser::get_argument_after({cmd}), _force);
            break;

        case commands::REMOVE:
            remove(argvparser::get_argument_after({cmd}), _force);
            break;
        case commands::RUN:
            run();
            break;
        case commands::TEMPLATE:
            use_template(argvparser::get_argument_after({cmd}), fs::current_path());
            break;

        case commands::SEARCH:
            search(repo(argvparser::get_argument(2), argvparser::get_argument(3)));
            break;
        case commands::EXIST:
            exist(argvparser::get_argument_after({cmd}));
            break;
        case commands::LS:
            ls();
            break;

        case commands::INFO:
            info(argvparser::get_argument_after({cmd}));
            break;

        case commands::GIT:
            git(argvparser::get_argument_after({cmd}));
            break;

        case commands::REPORT:
            report(argvparser::get_argument_after({cmd}));
            break;

        case commands::WEBSITE:
            website(argvparser::get_argument_after({cmd}));
            break;

        case commands::UNKNOWN:
            if (!argvparser::is_argument(cmd))clif::log(FATAL, "unknown command: " + cmd);
        default:
            break;
    }



    return 0;
}