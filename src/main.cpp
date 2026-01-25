#include <cstdlib>
#include <filesystem>
#include <functional>
#include <iomanip>
#include <iostream>
#include <string>
#include <unordered_map>
#include <vector>

#include "../include/appf.h"
#include "../include/argvf.h"
#include "../include/clif.h"
#include "../include/constants.h"
#include "../include/transactionf.h"

namespace fs = std::filesystem;
using namespace transactionf;
using namespace argvparser;

enum class commands {
  PURGE,
  INIT,
  INSTALL,
  REMOVE,
  RUN,
  TEMPLATE,
  SEARCH,
  EXIST,
  LS,
  TYPE,
  INFO,
  GIT,
  WEBSITE,
  REPORT,
  UNKNOWN
};
static const std::unordered_map<std::string, commands> commandMap = {
    {"purge", commands::PURGE},
    {"init", commands::INIT},
    {"install", commands::INSTALL},
    {"run", commands::RUN},
    {"remove", commands::REMOVE},
    {"template", commands::TEMPLATE},
    {"search", commands::SEARCH},
    {"ls", commands::LS},
    {"type", commands::TYPE},
    {"info", commands::INFO},
    {"git", commands::GIT},
    {"report", commands::REPORT},
    {"website", commands::WEBSITE},
    {"exist", commands::EXIST}

};

commands command(std::string &command) {
  auto it = commandMap.find(command);
  return (it != commandMap.end()) ? it->second : commands::UNKNOWN;
}
std::string repo(const std::string &user, const std::string &repo) {
  return ("https://github.com/" + user + "/" + repo + ".git");
}
int main(int argc, char *argv[]) {
  if (!fs::exists(HOME_DIRECTORY)) {
    fs::create_directories(HOME_DIRECTORY / "_sys" / "logs");
    fs::create_directories(HOME_DIRECTORY / "_sys" / "templates");
    fs::create_directories(HOME_DIRECTORY / "_sys" / "registry");
  }

  argvparser::init(argc, argv);
  bool _force = false;
  bool _url = false;
  bool _lib = false;
  bool _local = false;

  add_help("init", "initalizon the cclm project");
  add_help("install", "install library\t\t\t(supports the '-f' flag)");
  add_help("remove", "remove library\t\t\t(supports the '-f' flag)");
  add_help("run", "compile and run your project");
  add_help("template", "creates a template based on the name ");
  add_help("search", "checks the repository is available");
  add_help("exist", "checks if the library is installed");
  add_help("ls", "print a list of installed libraries");
  add_help("info", "print package information");
  add_help("type", "print what type is the ccfile.yml");
  add_help("git", "git command wrapper (for debugging purposes)");
  add_help("report", "report a library");
  add_help("website", "open websites of the library");

  add_help("purge", "uninstalls the application and removes all its traces.");

  define_argument(
      {"-u", "--url"}, [&_url]() { _url = true; },
      "takes the raw url address instead of the author and package name");
  define_argument(
      {"-f", "--force"}, [&_force]() { _force = true; },
      "executes the commands without question");
  define_argument(
      {"-L", "--lib"}, [&_lib]() { _lib = true; },
      "switch for init mode to library");
  define_argument(
      {"-l", "--local"}, [&_local]() { _local = true; },
      "switch for install to local mode");
  define_argument(
      {"-v", "--version"}, []() { clif::log(INFO, std::string(VERSION)); },
      "shows the current CCLM versions");
  define_argument(
      {"-d", "--debug"}, []() { DEBUG_MODE = true; }, "switch to debug mode");

  argvparser::parser();

  std::string cmd;
  if (!argvparser::has_argument(1)) {
    clif::log(ERROR, "You don't entered a commands. ");
    clif::log(HINT, "use the --help or -h to display the commands list.");
    return 1;
  }
  cmd = argvparser::get_argument(1);

  switch (command(cmd)) {
  case commands::PURGE:
    appf::purge(_force);
    break;

  case commands::INIT:
    init(_lib);
    break;

  case commands::INSTALL:
    if (!_url && !_local && argvparser::has_argument(3)) {
      install(repo(argvparser::get_argument(2), argvparser::get_argument(3)),
              _force);
      break;
    }
    install(argvparser::get_argument_after({cmd}), _force, _local);
    break;

  case commands::REMOVE:
    remove(argvparser::get_argument_after({cmd}), _force);
    break;
  case commands::RUN:
    run();
    break;
  case commands::TEMPLATE:
    use_template(argvparser::get_argument_after({cmd}));
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

  case commands::TYPE:
    type();
    break;

  case commands::REPORT:
    report(argvparser::get_argument_after({cmd}));
    break;

  case commands::WEBSITE:
    website(argvparser::get_argument_after({cmd}));
    break;

  case commands::UNKNOWN:
    if (!argvparser::is_argument(cmd))
      clif::log(FATAL, "unknown command: " + cmd);
  default:
    break;
  }

  return 0;
}
