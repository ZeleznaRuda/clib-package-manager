#include "../include/appf.h"
namespace appf {
void app_init() {
    if (fs::exists(homeDirectory)) {clif::log(WARN, "CLIBX is already initializatied.");return;}
    try {
        if (fs::create_directories(homeDirectory / "_sys") && fs::create_directories(homeDirectory / "_sys" / "templates") && fs::create_directories(homeDirectory / "_sys" / "libRecords")  ) {
            std::ofstream READMEFile(homeDirectory / "README.md");
            READMEFile << "DO NOT MODIFY THIS FOLDER" << std::endl;
            READMEFile.close();
            std::ofstream _configFile(homeDirectory / "_sys" / "config.yaml");
            _configFile << "git-path: git" << std::endl;
            _configFile.close();
        for (const auto& [filename, content] : templates::data) {
            std::ofstream file(homeDirectory / "_sys" / "templates" / filename);
            if (!file.is_open()) clif::log(FATAL, "cannot open file", 1);
            file << content;
            file.close();
        }
        } else {clif::log(FATAL,"initialization failed");}
        clif::log(INFO,"CLIBX has been successfully initialized");
    } catch (const fs::filesystem_error& e) {
        clif::log(FATAL,e.what());
    }
}// _lib-records
void app_clean(const bool force){
    if (!force) {
        if (!clif::confirm("Are you sure you want to delete the .clibx folder? (this will delete all libraries and records)")) {
            clif::log(INFO,"removing cancelled by user");
            return;
        }
    }
    try {
        if (fs::exists(homeDirectory)) fs::remove_all(homeDirectory);
        clif::log(INFO,".clibx was removed");
    } catch (const fs::filesystem_error& e) {
        clif::log(ERROR, "error removing .clibx: " + std::string(e.what()));
    }
}
void app_uninstall(const bool force) { // apk-uninstall
    if (!force) {
        if (!clif::confirm("Are you sure you want to uninstall CLIBX?")) {
            clif::log(INFO,"uninstallation cancelled by user");
            return;
        }
    }

    fs::path Path = userHomeDirectory / ".local" / "bin" / "clibx";

    if (!fs::exists(Path)) {
        clif::log(FATAL, "CLIBX does not exist.",2);
    }

    try {
        if (fs::exists(Path)) fs::remove(Path);
            clif::log(INFO,"\033[32mthe program has been successfully uninstalled, SAY GOODBYE\033[0m");
    } catch (const fs::filesystem_error& e) {
        clif::log(FATAL, "error removing CLIBX: " + std::string(e.what()),2);
    }
}
void app_config(const std::string& key, const std::string& content, const bool force){
    if (!force) {
        if (!clif::confirm("Do you really want to change the configuration?")) {
            clif::log(INFO,"changeing configuration cancelled by user");
            return;
        }
    }
    configf::changeConfig(key,content);
}
}