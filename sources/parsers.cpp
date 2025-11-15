#include "../include/parsers.h"
namespace yaml
{
    std::string read(const fs::path& fileName) {
        if (!fs::exists(fileName)) cli::log(FATAL, "file " + fileName.string() + " not found", -1);

        std::ifstream file(fileName);
        if (!file.is_open()) cli::log(FATAL, "cannot open file", -1);


        return std::string((std::istreambuf_iterator<char>(file)),
                           std::istreambuf_iterator<char>());
    }

    std::unordered_map<std::string, std::string> parser(const std::string& fileContent) {
        std::unordered_map<std::string, std::string> data;
        auto lines = utils::split(fileContent, '\n');

        for (auto& l : lines) {
            l = utils::strip(l);
            if (l.empty() || l[0] == '#') continue;

            auto hashPos = l.find('#');
            if (hashPos != std::string::npos)
                l = l.substr(0, hashPos);

            l = utils::strip(l);
            if (l.empty()) continue;

            auto parts = utils::split(l, ':');
            if (parts.size() >= 2) {
                std::string key = utils::strip(parts[0]);
                std::string value = utils::strip(l.substr(l.find(':') + 1));
                data[key] = value;
            }
        }

        return data;
    }
}


namespace lister
{
    std::string read(const fs::path& fileName) {
        if (!fs::exists(fileName)) cli::log(FATAL, "file " + fileName.string() + " not found", -1);
        std::ifstream file(fileName);
        if (!file.is_open()) cli::log(FATAL, "cannot open file", -1);
        return std::string((std::istreambuf_iterator<char>(file)), std::istreambuf_iterator<char>());
    }

    std::vector<std::string> parser(const std::string& fileContent) {
        std::vector<std::string> urls;
        std::istringstream stream(fileContent);
        std::string line;
        while (std::getline(stream, line)) {
            line = utils::strip(line);
            if (!line.empty()) urls.push_back(line);
        }
        return urls;
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
                cli::log(ERROR, "\033[1;91munknown argument:\033[0m " + arg + "\n");
            }
        }
    }
}