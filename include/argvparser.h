#pragma once
#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <regex>
#include <vector>
#include <algorithm>
#include <unordered_map>
#include <filesystem>
#include <cstdlib>
#include <iomanip>
#include <functional>
#include <cctype>
#include <variant>

#include "../include/toolsf.h"
#include "../include/clif.h"

namespace fs = std::filesystem;

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
    bool is_argument(const std::string name);
    std::string get_argument_after(const std::vector<std::string>& names, const std::string defaultValue = "");
    void parser(int i = 1);

    namespace {
        inline void help() {
            clif::log(INFO,"usage: clibx <options> [flag]\n");

            std::unordered_map<std::string, std::vector<std::string>> groupedFlags;

            for (const auto& kv : descriptionsArguments) {
                if (!kv.first.empty() && kv.first[0] == '-') {
                    groupedFlags[kv.second].push_back(kv.first);
                }
            }

            if (!groupedFlags.empty()) {
                clif::log(INFO,"flags:");
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
                clif::log(INFO,"options:");
                for (const auto& kv : options) {
                    int spacing = 20 - kv.first.length();
                    if (spacing < 1) spacing = 1;
                    std::cout << "  " << kv.first << std::string(spacing, ' ') << kv.second << "\n";
                }
            }
            clif::log(NONE,"\nMade by Rudolf Mueller\n");
        }



        inline bool starts_with(const std::string& text, const std::string& prefix) {
            return text.rfind(prefix, 0) == 0;
        }
    }
}
