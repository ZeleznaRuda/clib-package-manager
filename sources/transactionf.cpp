#include "../include/transactionf.h"

namespace transactionf
{
std::string metadataFileName = std::string(
    std::getenv("CLIBX_LIBRARY_METADATA_FILE_NAME") 
    ? std::getenv("CLIBX_LIBRARY_METADATA_FILE_NAME")  
    : "package.yml"
);
std::string gitPath = std::string(std::getenv("CLIBX_GIT_PATH") ? std::getenv("CLIBX_GIT_PATH") : "git");

std::string gccPath = std::string(std::getenv("CLIBX_GCC_PATH") ? std::getenv("CLIBX_GCC_PATH") : "gcc");
std::string gxxPath = std::string(std::getenv("CLIBX_GXX_PATH") ? std::getenv("CLIBX_GXX_PATH") : "g++");

std::string arPath  = std::string(std::getenv("CLIBX_AR_PATH")  ? std::getenv("CLIBX_AR_PATH")  : "ar");

void install(const std::string& url, const bool force, const bool installDependencies) {
    if (sysf({gitPath,"ls-remote",stringf::escape(url), "> /dev/null 2>&1"}) == 0) {
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

    fs::path tmpPath = HOME_DIRECTORY / "_sys" / (".tmp");
    if (!fs::create_directory(tmpPath)) {
        clif::log(FATAL, "install failed");
    }
    int cloneResult = sysf({gitPath, "clone", "--depth", "1", stringf::escape(url), stringf::escape(tmpPath.string()), "> /dev/null 2>&1"});
    auto infoData = yaml::parser(yaml::read(tmpPath / metadataFileName));

    if (cloneResult != 0) {
        clif::log(FATAL, "git clone failed with code " + std::to_string(cloneResult), cloneResult);
    } else {
        try {
            fs::path pkgPath = HOME_DIRECTORY / (std::get<std::string>(infoData["name"]) + "@" + std::get<std::string>(infoData["version"]));
            fs::create_directory(pkgPath);
            fs::create_directory(pkgPath / "build");
            fs::create_directory(pkgPath / "include");

            if (infoData.find("build-dependencies") != infoData.end()) {
                for (const auto& dep : std::get<std::vector<std::string>>(infoData["build-dependencies"])) {
                    exist(dep);
                }
            }

            std::string compilerPath = (std::get<std::string>(infoData["build-compiler"]) == "gcc") ? gccPath :
                                       (std::get<std::string>(infoData["build-compiler"]) == "g++") ? gxxPath : "";
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
                    "> /dev/null 2>&1"
                    
                };
                argv.insert(argv.begin() + 2, cflags.begin(), cflags.end());

                sysf(argv);

                
            }
            if (mode == "shared"){
                std::vector<std::string> args = { compilerPath, "-shared", "-o", (pkgPath / ("lib" + name + ".so")).string() };
                for (const auto& entry : fs::directory_iterator(pkgPath / "build")) {
                    if (entry.is_regular_file() && entry.path().extension() == ".o") {
                        args.push_back(entry.path().string());
                    }
                }
                args.push_back("> /dev/null 2>&1");
                sysf(args);
            }
            if (mode == "static"){
                std::vector<std::string> args = { arPath, "rcs", (pkgPath / ("lib" + name + ".a")).string() };
                for (const auto& entry : fs::directory_iterator(pkgPath / "build")) {
                    if (entry.is_regular_file() && entry.path().extension() == ".o") {
                        args.push_back(entry.path().string());
                    }
                }
                args.push_back("> /dev/null 2>&1");
                sysf(args);
            }

            fs::copy(tmpPath / includeDirectory, pkgPath / "include");
            fs::remove_all(tmpPath);

            std::ofstream pkgFile(HOME_DIRECTORY / "_sys" / "registry" / (name + "@" + std::get<std::string>(infoData["version"]) + "-package.yml"));
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
            clif::log(ERROR, std::string("error: ") + e.what() + "\n");
        }

        clif::log(INFO, "library installed successfully");
    }
}

void remove(const std::string& pkgName, bool force) {
    fs::path pkgPath = HOME_DIRECTORY / pkgName;
    fs::path pkgInfoFilePath = HOME_DIRECTORY / "_sys" / "registry" / (pkgName + "-package.yml");

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
        if (fs::exists(pkgInfoFilePath)) fs::remove(pkgInfoFilePath);
        clif::log(INFO,"library '" + pkgName + "' removed successfully.");
    } catch (const fs::filesystem_error& e) {
        clif::log(FATAL, "error removing library: " + std::string(e.what()),2);
    }
}

void connect(const std::string& pkgName, const fs::path& targetDirectory, const bool all) {
    std::vector<fs::path> packages;

    if (all) {
        for (auto& entry : fs::directory_iterator(HOME_DIRECTORY)) {
            if (fs::is_directory(entry.path())) {
                std::string name = entry.path().filename().string();
                if (name.empty() || name[0] == '_') continue;
                packages.push_back(entry.path());
            }
        }
    } else {
        fs::path pkgPath = HOME_DIRECTORY / pkgName;
        if (!fs::exists(pkgPath)) {
            clif::log(ERROR,"library " + pkgName +" not found\n");
        }
        packages.push_back(pkgPath);
    }

    for (auto& pkgPath : packages) {
        std::string name = pkgPath.filename().string();
        fs::path projectDirectory = targetDirectory / "libs" / name;

        fs::create_directories(projectDirectory.parent_path());

        if (fs::exists(projectDirectory) || fs::is_symlink(projectDirectory)) {
            std::error_code ec_remove;
            if (fs::remove(projectDirectory, ec_remove)) {
                clif::log(INFO,"disconnecting the project: " + projectDirectory.string() + "\n");
            } else {
                clif::log(ERROR,"error disconnecting the project: "+ ec_remove.message()+"\n");
            }
        }

        std::error_code ec;
        fs::create_directory_symlink(pkgPath, projectDirectory, ec);
        if (ec) {
            clif::log(FATAL,"error occurred when connecting project: " + ec.message() +"\n");
        } else {
            clif::log(INFO,"package '"+ projectDirectory.string() +"' was successfully connected\n");
        }
    }
}

void use_template(const std::string& name, const std::filesystem::path& targetDirectory) {
    if (!name.empty() && fs::exists(HOME_DIRECTORY / "_sys" / "templates" / name)) {
        std::ifstream file(HOME_DIRECTORY / "_sys" / "templates" / name);
        if (!file) {
            clif::log(FATAL, "failed to open template: " + name, 2);
            return;
        }

        std::ofstream templateFile(targetDirectory / name);
        if (!templateFile) {
            clif::log(FATAL, "failed to create template: " + targetDirectory.string(), 2);
            return;
        }

        std::string line;
        while (std::getline(file, line)) {
            templateFile << line << '\n';  
        }

        clif::log(INFO, "template successfully created: " + (targetDirectory / name).string());
    } else {
        clif::log(INFO,"templates:");
        for (const auto& entry : fs::directory_iterator(HOME_DIRECTORY / "_sys" / "templates")) {
            if (!fs::is_regular_file(entry.path())) continue;
            std::cout << "\t" << entry.path().filename().string() << '\n';
        }
        clif::log(ERROR, "template '" + name + "' does not exist");
    }
}

void search(const std::string& url){
    int result = sysf({gitPath, "ls-remote", stringf::escape(url), "> /dev/null 2>&1"});
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

void git(const std::string& command){
    sysf({gitPath, command});
}

void info(const std::string& repoName){
    const std::string suffix = "-package.yml";
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
