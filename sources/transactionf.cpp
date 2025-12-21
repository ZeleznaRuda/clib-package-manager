#include "../include/transactionf.h"

namespace transactionf
{


void install(const std::string& url, const bool force) {
    if (sysf({GIT_PATH,"ls-remote",stringf::escape(url)}).first == 0) {
        clif::log(INFO, "the library is accessible");
    } else {
        clif::log(FATAL, "library is not accessible");
    }

    if (!(stringf::starts_with(url, "http") || stringf::starts_with(url, "git@")) || !stringf::ends_with(url, ".git")) {
        clif::log(FATAL, "invalid git url format");
    }

    if (!force) {
        if (!clif::confirm("Are you sure you want to install the library from '" + url + "'")) {
            clif::log(INFO, "installation cancelled by user");
            return;
        }
    }
    fs::path tmpPath = HOME_DIRECTORY / "_sys" / (std::to_string(std::time(nullptr)) + ".tmp");
    if (!fs::create_directory(tmpPath)) {
        clif::log(FATAL, "install failed");
    }
    int cloneResult = sysf({GIT_PATH, "clone", "--depth", "1", stringf::escape(url), stringf::escape(tmpPath.string())}).first;
    yaml_t infoData = yaml::parser(yaml::read(tmpPath / PACKAGE_FILE));

    const std::vector<std::string> requiredKeys = {
        "name", "version", "description", "build-compiler",
        "build-mode", "build-include-directory", "build-source-files"
    };
    for (const auto& key : requiredKeys) {
        if (infoData.find(key) == infoData.end()) {
            clif::log(FATAL, "metadata missing key: " + key);
            return ;
        }
    } 

    if (cloneResult != 0) {
        clif::log(FATAL, "git clone failed with code " + std::to_string(cloneResult), cloneResult);
    } else {
        try {
            fs::path pkgPath = HOME_DIRECTORY / (std::get<std::string>(infoData["name"]) + "@" + std::get<std::string>(infoData["version"]));
            if (fs::exists(pkgPath)){
                fs::remove_all(pkgPath);
            }
            fs::create_directory(pkgPath);
            fs::create_directory(pkgPath / "build");
            fs::create_directory(pkgPath / "include");

            if (infoData.find("build-dependencies") != infoData.end()) {
                for (const auto& dep : std::get<std::vector<std::string>>(infoData["build-dependencies"])) {
                    exist(dep);
                }
            }

            std::string compilerPath = (std::get<std::string>(infoData["build-compiler"]) == "gcc") ? GCC_PATH :
                                       (std::get<std::string>(infoData["build-compiler"]) == "g++") ? GXX_PATH : "";
            std::string name = std::get<std::string>(infoData["name"]);
            std::string mode = std::get<std::string>(infoData["build-mode"]);
            std::string includeDirectory = (tmpPath / std::get<std::string>(infoData["build-include-directory"])).string();
            std::vector<std::string> cflags = 
                std::holds_alternative<std::vector<std::string>>(infoData["build-cflags"])
                ? std::get<std::vector<std::string>>(infoData["build-cflags"])
                : std::vector<std::string>{ std::get<std::string>(infoData["build-cflags"]) };
            
            for (const auto& file : std::get<std::vector<std::string>>(infoData["build-source-files"])) {
                if (!fs::exists(tmpPath / file)) {
                    clif::log(FATAL, "source file '" + (tmpPath / file).string() + "' not found", 1);
                }
                std::vector<std::string> argv = {
                    compilerPath,
                    "-fPIC",
                    "-I" + includeDirectory,
                    "-c",
                    (tmpPath / file).string(),
                    "-o",
                    (pkgPath / "build" / (fs::path(file).stem().string() + ".o")).string(), 
                    
                    
                };
                argv.insert(argv.begin() + 1, cflags.begin(), cflags.end());

                sysf(argv);

                
            }
            if (mode == "shared"){
                std::vector<std::string> args = { compilerPath, "-shared", "-o", (pkgPath / ("lib" + name + ".so")).string() };
                for (const auto& entry : fs::directory_iterator(pkgPath / "build")) {
                    if (entry.is_regular_file() && entry.path().extension() == ".o") {
                        args.push_back(entry.path().string());
                    }
                }
                sysf(args);
            }
            if (mode == "static"){
                std::vector<std::string> args = { AR_PATH, "rcs", (pkgPath / ("lib" + name + ".a")).string() };
                for (const auto& entry : fs::directory_iterator(pkgPath / "build")) {
                    if (entry.is_regular_file() && entry.path().extension() == ".o") {
                        args.push_back(entry.path().string());
                    }
                }
                sysf(args);
            }
            fs::copy(tmpPath / includeDirectory, pkgPath / "include");
            fs::remove_all(tmpPath);

            std::ofstream pkgFile(HOME_DIRECTORY / "_sys" / "registry" / (name + "@" + std::get<std::string>(infoData["version"]) + "-metadata.yml"));
            if (!pkgFile) {
                clif::log(FATAL, "failed to create record", 2);
            } else {
                std::time_t t = std::time(nullptr);
                std::tm* now = std::localtime(&t);

                pkgFile << "# package information:\n";
                pkgFile << "name: " << name << "\n";
                pkgFile << "description: " << std::get<std::string>(infoData["description"]) << "\n";
                if (infoData.find("version") != infoData.end()) {
                    pkgFile << "version: " << std::get<std::string>(infoData["version"]) << "\n";
                }
                if (infoData.find("authors") != infoData.end()) {
                    pkgFile << "authors: [" << stringf::join(std::get<std::vector<std::string>>(infoData["authors"]), ", ") << "]\n";
                } else {
                    pkgFile << "authors: unknown\n";
                }

                pkgFile << "\n# links:\n";
                if (infoData.find("websites") != infoData.end()) {
                    pkgFile << "websites: [" << stringf::join(std::get<std::vector<std::string>>(infoData["websites"]), ", ") << "]\n";
                }

                pkgFile << "\n# legal:\n";
                if (infoData.find("license") != infoData.end()) {
                    pkgFile << "license: " << std::get<std::string>(infoData["license"]) << "\n";
                }

                pkgFile << "\n# origin:\n";
                pkgFile << "git-url: " << url << "\n";
                pkgFile << "installation-date: " << std::put_time(now, "%d.%m.%Y-%H:%M:%S") << "\n";

                if (infoData.find("dependencies") != infoData.end()) {
                    pkgFile << "\n# dependencies:\n";
                    pkgFile << "dependencies: [" << stringf::join(std::get<std::vector<std::string>>(infoData["dependencies"]), ", ") << "]\n";
                    for (const auto& dep : std::get<std::vector<std::string>>(infoData["dependencies"])) {
                        exist(dep);
                    }
                }

                pkgFile.close();
                clif::log(INFO, "record successfully created");
            }

        } catch (fs::filesystem_error& e) {
            clif::log(FATAL, std::string("error: ") + e.what() + "\n");
        } 

    }
    clif::log(INFO, "library installed successfully");

}

void remove(const std::string& pkgName, bool force) {
    fs::path pkgPath = HOME_DIRECTORY / pkgName;
    if (fs::exists(pkgPath) ) {
        clif::log(INFO,"library '" + pkgName + "' is installed.");
    } else {
        clif::log(FATAL,"library '" + pkgName + "' is \033[1;31mnot\033[0m installed.");
    }
    fs::path infoPath = HOME_DIRECTORY / "_sys" / "registry" / (pkgName + "-metadata.yml");

    if (!fs::exists(pkgPath)) {
        clif::log(FATAL, "library '" + pkgName + "' does not exist.");
    }

    if (!force) {
        if (!clif::input("To continue, you must rewrite the package name and enter it. [package name]: ").compare(pkgName) == 0) {
            clif::log(INFO,"removal cancelled by force");
            return;
        }
        if (!clif::confirm("Are you sure you want to remove '" + pkgName + "' library?")) {
            clif::log(INFO,"removal cancelled by user");
            return;
        }
    }

    try {
        if (fs::exists(pkgPath)) fs::remove_all(pkgPath);
        if (fs::exists(infoPath)) fs::remove(infoPath);
        clif::log(INFO,"library '" + pkgName + "' removed successfully.");
    } catch (const fs::filesystem_error& e) {
        clif::log(FATAL, "error removing library: " + std::string(e.what()),2);
    }
}

void run() {
    if (!fs::exists(CURRENT_PATH / BUILD_FILE)) {
        clif::log(FATAL, "build file not found");
    }

    yaml_t buildData = yaml::parser(yaml::read(CURRENT_PATH / BUILD_FILE));
    const std::vector<std::string> requiredKeys = {
       "name", "sources-files", "output-directory", "compiler"
    };

    for (const auto& key : requiredKeys) {
        if (buildData.find(key) == buildData.end()) {
            clif::log(FATAL, "metadata missing key: " + key);
            return;
        }
    }

    try {
        std::string name = std::get<std::string>(buildData["name"]);

        std::vector<std::string> librarys;
        if (std::holds_alternative<std::vector<std::string>>(buildData["library"])) {
            librarys = std::get<std::vector<std::string>>(buildData["library"]);
        } else {
            librarys = {}; 
        }


        std::string compiler = (std::get<std::string>(buildData["compiler"]) == "gcc") ? GCC_PATH :
                               (std::get<std::string>(buildData["compiler"]) == "g++") ? GXX_PATH : "";
        if (compiler.empty()) clif::log(FATAL, "unsupported compiler");

        std::vector<std::string> sources = 
            std::holds_alternative<std::vector<std::string>>(buildData["sources-files"])
            ? std::get<std::vector<std::string>>(buildData["sources-files"])
            : std::vector<std::string>{ std::get<std::string>(buildData["sources-files"]) };

        fs::path output = CURRENT_PATH / std::get<std::string>(buildData["output-directory"]) / name;

        std::vector<std::string> cflags = 
                std::holds_alternative<std::vector<std::string>>(buildData["cflags"])
                ? std::get<std::vector<std::string>>(buildData["cflags"])
                : std::vector<std::string>{ std::get<std::string>(buildData["cflags"]) };

        std::vector<std::string> argv = {
            compiler
        };
        argv.insert(argv.end(), sources.begin(), sources.end());
        if (!librarys.empty()){
            for (const auto& lib : librarys) {
                fs::path libDirectory = HOME_DIRECTORY / lib;
                std::string library = stringf::split(libDirectory.filename().string(), '@')[0];

                argv.push_back("-I" + (libDirectory / "include").string());

                argv.push_back("-L" + libDirectory.string());
                argv.push_back("-l" + library);
                argv.push_back("-Wl,-rpath," + libDirectory.string());
                argv.push_back("-Wl,--enable-new-dtags");
            }
        }
        argv.insert(argv.begin() + 1, cflags.begin(), cflags.end());


        argv.push_back("-o");
        argv.push_back(output.string());

        sysf_t result = sysf(argv);
        if (result.first != 0) {
            clif::log(FATAL, '\n' + result.second, 2);
        }

        clif::log(INFO, "compilation successfully");
        sysf({output});

        clif::log(INFO, "program run successfully");

    } catch (const fs::filesystem_error& e) {
        clif::log(FATAL, std::string("file system error: ") + e.what(), 2);
    }
}


void use_template(const std::string& name, const std::filesystem::path& targetDirectory) {
    if (!name.empty() && fs::exists(HOME_DIRECTORY / "_sys" / "templates" / name)) {
        fs::copy(HOME_DIRECTORY / "_sys" / "templates" / name, CURRENT_PATH);
        clif::log(INFO, "template successfully created: " + (targetDirectory / name).string());
    } else {

        clif::log(INFO,"templates:");
        for (const auto& entry : fs::directory_iterator(HOME_DIRECTORY / "_sys" / "templates")) {
            if (!fs::is_regular_file(entry.path())) continue;
            std::cout << "\t" << entry.path().filename().string() << '\n';
        }
        if (!name.empty()){
            clif::log(ERROR, "template '" + name + "' does not exist");
        }
    }
}

void search(const std::string& url){
    int result = sysf({GIT_PATH, "ls-remote", stringf::escape(url)}).first;
    if (result == 0){
        clif::log(INFO,"the library is accessible");
    } else {
        clif::log(WARN,"library is not accessible");
    }
}

void exist(const std::string& pkg){
    fs::path pkgPath = HOME_DIRECTORY / pkg;

    if (fs::exists(pkgPath) ) {
        clif::log(INFO,"library '" + pkg + "' is installed.");
    } else {
        clif::log(WARN,"library '" + pkg + "' is \033[1;31mnot\033[0m installed.");
    }
}

void report(const std::string& pkgName){
    if (!fs::exists(HOME_DIRECTORY / "_sys" / "registry" / (pkgName + "-metadata.yml"))){
        clif::log(FATAL, "library is not installed");
    }
    yaml_t infoData = yaml::parser(yaml::read(HOME_DIRECTORY / "_sys" / "registry" / (pkgName + "-metadata.yml")));
    std::string url = std::get<std::string>(infoData["git-url"]);
    if (stringf::ends_with(url, ".git")){
        url.erase(url.size() - 4);
    }
    sysf({"xdg-open",stringf::escape(url+"/issues/new/")});
}

void website(const std::string& pkgName){
    if (!fs::exists(HOME_DIRECTORY / "_sys" / "registry" / (pkgName + "-metadata.yml"))){
        clif::log(FATAL, "library is not installed");
    }
    yaml_t infoData = yaml::parser(yaml::read(HOME_DIRECTORY / "_sys" / "registry" / (pkgName + "-metadata.yml")));
    if (infoData.find("websites") == infoData.end()){
        clif::log(FATAL,"missing websites key");
    }
    std::vector<std::string> websites = std::get<std::vector<std::string>>(infoData["websites"]);

    for (auto& website : websites){
        sysf({"xdg-open",stringf::escape(website)});
    }
}

void git(const std::string& command){
    std::cout << sysf({GIT_PATH, command}).second << std::endl;
}

void info(const std::string& repoName){
    const std::string suffix = "-metadata.yml";
    std::string name = repoName;

    if (name.size() >= suffix.size() &&
            name.compare(name.size() - suffix.size(), suffix.size(), suffix) == 0) {
        name.erase(name.size() - suffix.size());
    }

    std::ifstream file(HOME_DIRECTORY / "_sys" / "registry" / (name + suffix)); 
    if (!file.is_open()) {
        clif::log(ERROR, "package does not exist");
    }

    std::string line;
    while (std::getline(file, line)) {  
        std::cout << line << std::endl;
    }
    file.close();  
}

void ls(){
    clif::log(INFO,"installed library:");

    for (const auto& entry : fs::directory_iterator(HOME_DIRECTORY)) {
        if (!fs::is_directory(entry.path())) continue;
        std::string name = entry.path().filename().string();
        if (name == "_sys") continue;
        std::cout << "-\t" << name << std::endl;
    }
}

} // namespace transactionf
