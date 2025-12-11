#include "../include/argvparserf.h"
namespace yaml
{
    
    std::string read(const fs::path& fileName) {
        if (!fs::exists(fileName)) clif::log(FATAL, "file " + fileName.string() + " not found", 1);

        std::ifstream file(fileName);
        if (!file.is_open()) clif::log(FATAL, "cannot open file", 1);

        return std::string((std::istreambuf_iterator<char>(file)),
                           std::istreambuf_iterator<char>());
    }

    std::unordered_map<std::string, std::variant<std::string, std::vector<std::string>>> parser(const std::string& fileContent) {
        std::unordered_map<std::string, std::variant<std::string, std::vector<std::string>>> data;
        auto lines = stringf::split(fileContent, '\n');

        for (auto& l : lines) {
            l = stringf::strip(l);
            if (l.empty() || l[0] == '#') continue;

            auto hashPos = l.find('#');
            if (hashPos != std::string::npos)
                l = l.substr(0, hashPos);

            l = stringf::strip(l);
            if (l.empty()) continue;

            auto parts = l.find(':');
            if (parts != std::string::npos) {
                std::string key = stringf::strip(l.substr(0, parts));
                std::string value = stringf::strip(l.substr(parts + 1));

                if (!value.empty() && value.front() == '[' && value.back() == ']') {
                    value = stringf::strip(value.substr(1, value.size() - 2));
                    auto items = stringf::split(value, ',');

                    std::vector<std::string> vectorContent;

                    for (auto &item : items) {
                        item = stringf::strip(item);
                        if (!item.empty() && item.front() == '"' && item.back() == '"') {
                            item = item.substr(1, item.size() - 2); 
                        }
                        if (!item.empty())
                            vectorContent.push_back(item);
                    }

                    data[key] = vectorContent;
                    continue;

                } else { 
                    if (!value.empty() && value.front() == '"' && value.back() == '"') {
                        value = value.substr(1, value.size() - 2);
                    }
                    data[key] = value;
                }
            }
        }

        return data;
    }

}

namespace argvparser
{
    void init(int _argc, char** _argv) {
        argc = _argc;
        argv = _argv;

        definedArguments["--help"] = [](){ help(); std::exit(0); };
        definedArguments["-h"] = [](){ help(); std::exit(0); };
        descriptionsArguments["--help"] = "show this help message and exit";
        descriptionsArguments["-h"] = "show this help message and exit";
    }

    void add_help(const std::string& name, const std::string& description) {
        descriptionsArguments[name] = description;
    }

    std::string get_argument(int index, const std::string defaultValue) {
        if (index < 0 || index >= argc) return defaultValue;
        return std::string(argv[index]);
    }
    std::string argument(const std::vector<std::string>& options, const std::string& defaultValue) {
        for (int i = 0; i < argc; ++i) {
            std::string arg = argv[i];
            for (const auto& opt : options) {
                if (arg == opt) return arg;
            }
        }
        return defaultValue;
    }
    bool is_argument(const std::string name){
        if (starts_with(name, "--") || starts_with(name, "-")){
            return true;
        }
        return false;

    }
    bool has_argument(int index) {
        return argv[index] != nullptr;
    }

    std::string get_argument_after(const std::vector<std::string>& names, const std::string defaultValue) {
        for (int i = 0; i < argc; ++i) {
            for (const auto& name : names) {
                if (argv[i] == name && i + 1 < argc) return std::string(argv[i + 1]);
            }
        }
        return defaultValue;
    }

    void parser(int i) {
        for (; i < argc; ++i) {
            std::string arg = argv[i];
            if (definedArguments.find(arg) != definedArguments.end()) {
                definedArguments[arg]();
            } else if (starts_with(arg, "--") || starts_with(arg, "-")) {
                clif::log(FATAL, "unknown argument: " + arg + "\n");
            }
        }
    }
}