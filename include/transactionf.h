#pragma once
#include <iostream>
#include <cstdlib>
#include <string>
#include <filesystem>
#include <vector>
#include <sstream>
#include <fstream>
#include <unordered_map>
#include <ctime>
#include <iomanip>
#include <regex>

#include "../include/configf.h"
#include "../include/constants.h"
#include "../include/utilsf.h"
#include "../include/parsers.h"
#include "../include/clif.h"
namespace transactionf
{

    void install(const std::string& url, bool force = false, const bool installDependencies = true);
    void uninstall(const std::string& pkgName, bool force = false);
    void connect(const std::string& pkgName, const std::filesystem::path& targetDirectory, const bool all);
    void ctemplate(const std::string& name, const std::filesystem::path& targetDirectory);
    void search(const std::string& repoName);
    void info(const std::string& repoName);
    void ls();

} // namespace 
