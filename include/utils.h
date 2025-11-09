#pragma once

#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <vector>
#include <algorithm>
#include <unordered_map>
#include <filesystem>
#include <cstdlib>
#include <iomanip>
#include <functional>
#include <cctype>
#include <variant>

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
            std::cout << "Usage: clibx" << " [options]\n\n";

            std::vector<std::pair<std::string, std::string>> flags;
            std::vector<std::pair<std::string, std::string>> options;

            for (const auto& kv : descriptionsArguments) {
                if (!kv.first.empty() && kv.first[0] == '-') {
                    flags.push_back(kv);
                } else {
                    options.push_back(kv);
                }
            }

            auto cmp_length = [](const auto& a, const auto& b) {
                return a.first.length() < b.first.length();
            };

            std::sort(flags.begin(), flags.end(), cmp_length);
            std::sort(options.begin(), options.end(), cmp_length);

            if (!flags.empty()) {
                std::cout << "Flags:\n";
                for (const auto& kv : flags) {
                    int spacing = 20 - kv.first.length();
                    if (spacing < 1) spacing = 1;
                    std::cout << "  " << kv.first << std::string(spacing, ' ') << kv.second << "\n";
                }
                std::cout << "\n";
            }

            if (!options.empty()) {
                std::cout << "Options:\n";
                for (const auto& kv : options) {
                    int spacing = 20 - kv.first.length();
                    if (spacing < 1) spacing = 1;
                    std::cout << "  " << kv.first << std::string(spacing, ' ') << kv.second << "\n";
                }
            }
        }


        inline bool starts_with(const std::string& text, const std::string& prefix) {
            return text.rfind(prefix, 0) == 0;
        }
    }
}
