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
    void install(const std::string& url, bool force = false, const bool installDependencies = true);
    void remove(const std::string& pkgName, bool force = false);
    void connect(const std::string& pkgName, const std::filesystem::path& targetDirectory, const bool all);
    void use_template(const std::string& name, const std::filesystem::path& targetDirectory);
    void search(const std::string& repoName);
    void git(const std::string& command);
    void info(const std::string& repoName);
    void ls();
    void exist(const std::string& pkgName);
} // namespace transactionf
