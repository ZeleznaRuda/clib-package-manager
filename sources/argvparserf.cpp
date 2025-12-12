#include "../include/argvparserf.h"


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