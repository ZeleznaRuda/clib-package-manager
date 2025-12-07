#include "../include/transactionf.h"
namespace transactionf
{

std::string gitPath = std::string(std::getenv("CLIBX_GIT_PATH") ? std::getenv("CLIBX_GIT_PATH") : "git");
std::string metadataFileName = std::string(
    std::getenv("CLIBX_PACKAGE_METADATA_FILE_NAME") 
    ? std::getenv("CLIBX_PACKAGE_METADATA_FILE_NAME")  
    : "package.yml"
);
void install(const std::string& url, const bool force, const bool installDependencies) {

    if (system((gitPath + " ls-remote " + utilsf::escapeShellArg(url) + " &> /dev/null").c_str()) == 0) {
        clif::log(INFO, "the library is accessible");
    } else {
        clif::log(FATAL, "library is not accessible");
    }

    if (!force) {
        if (!clif::confirm("Are you sure you want to install the library from '" + url + "'")) {
            clif::log(INFO,"installation cancelled by user");
            return;
        }
    }

    if (!(utilsf::start_with(url, "http") || utilsf::start_with(url, "git@")) 
        || !utilsf::ends_with(url, ".git")) {
        clif::log(FATAL,"invalid git url format");
    }

    std::string repoName = url.substr(url.find_last_of('/') + 1);
    if (utilsf::ends_with(repoName, ".git"))
        repoName = repoName.substr(0, repoName.size() - 4);

    fs::path pkgPath = homeDirectory / "_sys" / (repoName + ".tmp");

    try {
        if (!fs::create_directory(pkgPath)) {
            clif::log(ERROR,"install failed");
        }
    } catch (const fs::filesystem_error& e) {
        clif::log(FATAL,e.what(), 2);
    }
    std::string command = gitPath + " clone --depth 1 " + utilsf::escapeShellArg(url) + " " + utilsf::escapeShellArg(pkgPath.string()) + " &> /dev/null";
    int result = system(command.c_str());
    fs::remove_all(pkgPath / ".git");
    auto infoData = yaml::parser(yaml::read(pkgPath / metadataFileName));
    
    std::ofstream pkgFile(homeDirectory / "_sys" / "libRecords" / (std::get<std::string>(infoData["name"]) + "@" + std::get<std::string>(infoData["version"]) + "-package.yml"));
    if (!pkgFile) {
        clif::log(FATAL,"failed to create record: " + (homeDirectory / "_sys" / "libRecords" / (std::get<std::string>(infoData["name"]) + "@" + std::get<std::string>(infoData["version"]) + "-package.yml")).string(),2);
    } else {
        std::time_t t = std::time(nullptr);
        std::tm* now = std::localtime(&t);
        pkgFile << "# package information:" << std::endl;
        pkgFile << "name: " << std::get<std::string>(infoData["name"]) << std::endl;
        pkgFile << "description: " << std::get<std::string>(infoData["description"]) << std::endl;
        if (infoData.find("version") != infoData.end()) {
            pkgFile << "version: " << std::get<std::string>(infoData["version"]) << std::endl;
        }
        if (infoData.find("authors") != infoData.end()) {
            pkgFile << "authors: [" << utilsf::join(std::get<std::vector<std::string>>(infoData["authors"]), ", ") << "]" << std::endl;
        } else {
            pkgFile << "authors: unknown" << std::endl;
        }
        pkgFile << std::endl;
        pkgFile << "# links:" << std::endl;
        if (infoData.find("websites") != infoData.end()) {
            pkgFile << "websites: [" << utilsf::join(std::get<std::vector<std::string>>(infoData["websites"]), ", ") << "]" << std::endl;
        }
        pkgFile << std::endl;
        pkgFile << "# legal:" << std::endl;
        if (infoData.find("license") != infoData.end()) {
            pkgFile << "license: " << std::get<std::string>(infoData["license"]) << std::endl;
        }

        pkgFile << std::endl;
        pkgFile << "# origin:" << std::endl;

        pkgFile << "git-url: " << url << std::endl;
        pkgFile << "installation-date: " << std::put_time(now, "%d.%m.%Y-%H:%M:%S") << std::endl;

        if (infoData.find("dependencies") != infoData.end()) {
            pkgFile << std::endl;
            pkgFile << "# dependencies:" << std::endl;
            pkgFile << "dependencies: [" << utilsf::join(std::get<std::vector<std::string>>(infoData["dependencies"]), ", ") << "]" << std::endl;
            for (const auto& dep : std::get<std::vector<std::string>>(infoData["dependencies"])) {
                exist(dep);
            }
        }
        pkgFile.close();
        clif::log(INFO,"record successfully created " + (homeDirectory / "_sys" / "libRecords" / (std::get<std::string>(infoData["name"]) + "@" + std::get<std::string>(infoData["version"]) + "-package.yml")).string());
    }

    if (result != 0) {
        clif::log(FATAL,"git clone failed with code " + std::to_string(result), result);
    } else {
        try {
            fs::path newPath = homeDirectory / fs::path(std::get<std::string>(infoData["name"])+ "@" + std::get<std::string>(infoData["version"]));
            if (fs::exists(newPath)) {
                fs::remove_all(pkgPath);
                clif::log(WARN, "the library is already installed.");
                return;
            }
            fs::rename(pkgPath.string(), newPath);
            pkgPath = newPath;
        } catch (fs::filesystem_error& e) {
            clif::log(ERROR,std::string("error: ") + e.what() + "\n");
        }
        clif::log(INFO,"library installed successfully to " + pkgPath.string());
    }

    fs::create_directories(homeDirectory / "_sys");


}


void remove(const std::string& pkgName, bool force) {

    fs::path pkgPath = homeDirectory / pkgName;
    fs::path pkgInfoFilePath = homeDirectory / "_sys" / "libRecords" / (pkgName + "-package.yml");

    if (!fs::exists(pkgPath)) {
        clif::log(FATAL, "library '" + pkgName + "' does not exist.");
    }

    if (!force) {
        if (!clif::input("To continue, you must rewrite the package name and enter it. [package name]: ") .compare(pkgName) == 0) {
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
        for (auto& entry : fs::directory_iterator(homeDirectory)) {
            if (fs::is_directory(entry.path())) {
                std::string name = entry.path().filename().string();
                if (name.empty() || name[0] == '_') {
                    continue; 
                }
                packages.push_back(entry.path());
            }
        }
    } else {
        fs::path pkgPath = homeDirectory / pkgName;
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
    if (!name.empty() && fs::exists(homeDirectory / "_sys" / "templates" / name)) {
        std::ifstream file(homeDirectory / "_sys" / "templates" / name);
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
        for (const auto& entry : fs::directory_iterator(homeDirectory / "_sys" / "templates")) {
            if (!fs::is_regular_file(entry.path())) continue;
            std::cout << "\t" << entry.path().filename().string() << '\n';
        }
        clif::log(ERROR, "template '" + name + "' does not exist");
    }
}

void search(const std::string& url){
    std::string command = gitPath + " ls-remote "+ utilsf::escapeShellArg(url)+" &> /dev/null";
    int result = system(command.c_str());
    if (result == 0){
        clif::log(INFO,"the library is accessible");
    } else {
        clif::log(WARN,"library is not accessible");
    }
}

void exist(const std::string& pkg){
    fs::path pkgPath = homeDirectory / pkg;

    if (fs::exists(pkgPath) ) {
        clif::log(INFO,"library '" + pkg + "' is installed.");
    } else {
        clif::log(WARN,"library '" + pkg + "' is \033[1;31mnot\033[0m installed.");
    }
}

void git(const std::string& command){
    exit(system((gitPath + " " + command).c_str()));
}
void info(const std::string& repoName){
    const std::string suffix = "-package.yml";

    std::string name = repoName;

    if (name.size() >= suffix.size() &&
            name.compare(name.size() - suffix.size(), suffix.size(), suffix) == 0) {
            name.erase(name.size() - suffix.size());
    }
    std::ifstream file(homeDirectory / "_sys" / "libRecords" / (name + suffix)); 
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

    for (const auto& entry : fs::directory_iterator(homeDirectory)) {
        if (!fs::is_directory(entry.path())) continue;
        std::string name = entry.path().filename().string();
        if (name == "_sys") continue;

        std::cout << "-\t" << name << std::endl;
    }
}
}