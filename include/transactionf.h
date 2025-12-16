#pragma once
#include <iostream>
#include <cstdlib>
#include <string>
#include <filesystem>
#include <vector>
#include <variant>
#include <sstream>
#include <fstream>
#include <unordered_map>
#include <ctime>
#include <iomanip>
#include <regex>

#include "../include/constants.h"
#include "../include/toolsf.h"
#include "../include/yamlparser.h"
#include "../include/clif.h"

namespace transactionf
{
    void install(const std::string& url, bool force = false);
    void remove(const std::string& pkgName, bool force = false);
    void use_template(const std::string& name, const std::filesystem::path& targetDirectory);
    void search(const std::string& repoName);
    void git(const std::string& command);
    void info(const std::string& repoName);
    void ls();
    void report(const std::string& url);
    void website(const std::string& pkgName);
    void exist(const std::string& pkgName);
} // namespace transactionf
