#pragma once
#include <cstdlib>
#include <ctime>
#include <filesystem>
#include <fstream>
#include <iomanip>
#include <iostream>
#include <regex>
#include <sstream>
#include <string>
#include <unordered_map>
#include <variant>
#include <vector>

#include "../include/clif.h"
#include "../include/constants.h"
#include "../include/toolsf.h"
#include "../include/yamlf.h"

namespace transactionf {
void init(const bool lib = false, const bool git = false);
void install(const std::string &url, bool force = false,
             const bool local = false);
void remove(const std::string &pkgName, bool force = false);
void run();
void use_template(const std::string &name);
void search(const std::string &repoName);
void git(const std::string &command);
void info(const std::string &repoName);
void clean();
void ls();
void type();
void report(const std::string &url);
void website(const std::string &pkgName);
void exist(const std::string &pkgName);
} // namespace transactionf
