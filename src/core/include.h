#pragma once
#include <string>
#include <filesystem>

namespace core {
    namespace console {
        void log(const std::string& name);
        void warn(const std::string& name);
        void err(const std::string& name);
    }
    void init();
    void install(const std::string& url, bool force = false);
    void uninstall(const std::string& pkgName, bool force = false);
    void connect(const std::string& pkgName, const std::filesystem::path& targetDirectory);
}
