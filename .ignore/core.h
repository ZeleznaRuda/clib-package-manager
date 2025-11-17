#pragma once
#include <string>
#include <filesystem>
#include <unordered_map>
#include "utils.h"

namespace core {
    inline std::filesystem::path homeDirectory = utilsf::getHomeDirectory() / ".clibx";

    void apk_init();
    void install(const std::string& url, bool force = false, const bool installDependencies = true);
    void uninstall(const std::string& pkgName, bool force = false);
    void connect(const std::string& pkgName, const std::filesystem::path& targetDirectory, const bool all);
    void ctemplate(const std::string& name, const std::filesystem::path& targetDirectory);
    void search(const std::string& repoName);
    void info(const std::string& repoName);
    void ls();
    void changeConfig(const std::string& key, const std::string& content);
    void apk_clean(const bool force = false);
    void apk_uninstall(const bool force = false);

}
