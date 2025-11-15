#include "../include/utils.h"

namespace utils
{
    fs::path getHomeDirectory() {
        const char* home = std::getenv("HOME");
        return home ? fs::path(home) : fs::path{};
    }

    std::string escapeShellArg(const std::string& arg) {
        std::string escaped = "'";
        for (char c : arg) {
            if (c == '\'')
                escaped += "'\\''";
            else
                escaped += c;
        }
        escaped += "'";
        return escaped;
    }

    bool ends_with(const std::string& str, const std::string& suffix) {
        return str.size() >= suffix.size() &&
               str.compare(str.size() - suffix.size(), suffix.size(), suffix) == 0;
    }

    bool start_with(const std::string& str, const std::string& prefix) {
        return str.size() >= prefix.size() &&
               str.compare(0, prefix.size(), prefix) == 0;
    }

    std::vector<std::string> split(const std::string& text, char delimiter) {
        std::vector<std::string> result;
        std::stringstream ss(text);
        std::string item;
        while (std::getline(ss, item, delimiter)) {
            result.push_back(item);
        }
        return result;
    }

    std::string strip(const std::string& s) {
        size_t start = 0, end = s.size();
        while (start < s.size() && std::isspace(static_cast<unsigned char>(s[start]))) ++start;
        while (end > start && std::isspace(static_cast<unsigned char>(s[end - 1]))) --end;
        return s.substr(start, end - start);
    }
}

namespace console
{
    void log(const std::string& name) {
        std::cout << "\033[92minfo\033[0m:\n└──────── " << name << std::endl;
    }

    void warn(const std::string& name) {
        std::cout << "\033[93mwarning\033[0m:\n└──────── " << name << std::endl;
    }

    void err(int exitCode, const std::string& name) {
        std::cerr << "\033[91merror\033[0m:\n└──────── " << name << std::endl;
        if (exitCode != -1) exit(exitCode);
    }
}

namespace yaml
{
    std::string read(const fs::path& fileName) {
        if (!fs::exists(fileName))
            console::err(-1, "file " + fileName.string() + " not found");

        std::ifstream file(fileName);
        if (!file.is_open())
            console::err(-1, "cannot open file");

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
        if (!fs::exists(fileName)) console::err(1, "file " + fileName.string() + " not found");
        std::ifstream file(fileName);
        if (!file.is_open()) console::err(1, "cannot open file");
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
        if (argc <= 1) help();
        for (; i < argc; ++i) {
            std::string arg = argv[i];
            if (definedArguments.find(arg) != definedArguments.end()) {
                definedArguments[arg]();
            } else if (starts_with(arg, "--") || starts_with(arg, "-")) {
                console::err(1, "\033[1;91munknown argument:\033[0m " + arg + "\n");
            }
        }
    }
}