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
#include "../include/utils.h"
#include "../include/cli.h"

namespace fs = std::filesystem;

namespace yaml
{
    std::string read(const fs::path& fileName);
    void write(fs::path filename, std::string& content);
    void writeln(fs::path filename,std::string& key, std::string& content);
    bool changeKey(const fs::path& filename, const std::string& key, const std::string& newValue);
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
    std::string argument(const std::vector<std::string>& options, const std::string& defaultValue = "") ;
    std::string get_argument_after(const std::vector<std::string>& names, const std::string defaultValue = "");
    void parser(int i = 1);

    namespace {
        inline void help() {
            cli::log(INFO,"usage: clibx [options]\n");

            std::unordered_map<std::string, std::vector<std::string>> groupedFlags;

            for (const auto& kv : descriptionsArguments) {
                if (!kv.first.empty() && kv.first[0] == '-') {
                    groupedFlags[kv.second].push_back(kv.first);
                }
            }

            if (!groupedFlags.empty()) {
                cli::log(INFO,"flags:");
                for (const auto& kv : groupedFlags) {
                    std::string names;
                    for (size_t i = 0; i < kv.second.size(); ++i) {
                        if (i != 0) names += ", ";
                        names += kv.second[i];
                    }

                    int spacing = 30 - static_cast<int>(names.length());
                    if (spacing < 1) spacing = 1;
                    std::cout << "  " << names << std::string(spacing, ' ') << kv.first << "\n";
                }
                std::cout << "\n";
            }

            std::vector<std::pair<std::string, std::string>> options;
            for (const auto& kv : descriptionsArguments) {
                if (kv.first.empty() || kv.first[0] != '-') {
                    options.push_back(kv);
                }
            }

            if (!options.empty()) {
                cli::log(INFO,"options:");
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
