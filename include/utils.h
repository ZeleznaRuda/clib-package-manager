#pragma once

#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <vector>
#include <unordered_map>
#include <filesystem>
#include <cstdlib>
#include <iomanip>
#include <functional>
#include <cctype>

namespace fs = std::filesystem;

namespace utils
{
    fs::path getHomeDirectory();
    std::string escapeShellArg(const std::string& arg);
    bool ends_with(const std::string& str, const std::string& suffix);
    bool start_with(const std::string& str, const std::string& prefix);
    std::vector<std::string> split(const std::string& text, char delimiter);
    std::string strip(const std::string& s);
}

namespace console
{
    void log(const std::string& name);
    void warn(const std::string& name);
    void err(int exitCode, const std::string& name);
}

namespace yaml
{
    std::string read(const fs::path& fileName);
    std::unordered_map<std::string, std::string> parser(const std::string& fileContent);
}

namespace lister
{
    std::string read(const fs::path& fileName);
    std::vector<std::string> parser(const std::string& fileContent);
}

namespace argvparser
{
    inline std::unordered_map<std::string, std::function<void()>> definedArguments;
    inline std::unordered_map<std::string, std::string> descriptionsArguments;
    inline int argc = 0;
    inline char** argv = nullptr;

    void init(int _argc, char** _argv);
    void add_help(const std::string& name, const std::string& description);

    template<typename Func>
    void define_argument(const std::vector<std::string>& names, const Func lambda, const std::string description = "")
    {
        for (const auto& name : names) {
            definedArguments[name] = lambda;
            descriptionsArguments[name] = description;
        }
    }

    std::string get_argument(int index, const std::string defaultValue = "");
    bool has_argument(int index);
    std::string get_argument_after(const std::vector<std::string>& names, const std::string defaultValue = "");
    void parser(int i = 1);

    namespace {
        inline void help() {
            int maxKeyLength = 0;
            for (const auto& [key, val] : descriptionsArguments) {
                if (key.length() > maxKeyLength) maxKeyLength = key.length();
            }
            for (const auto& [key, val] : descriptionsArguments) {
                std::cout << std::setw(maxKeyLength + 2) << std::left << key
                          << " - " << val << std::endl;
            }
        }

        inline bool starts_with(const std::string& text, const std::string& prefix) {
            return text.rfind(prefix, 0) == 0;
        }
    }
}
