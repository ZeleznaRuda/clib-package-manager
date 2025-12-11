#include "../include/appf.h"
namespace appf {
void purge(const bool force) { // apk-uninstall
    if (!force) {
        std::srand(static_cast<unsigned int>(std::time(nullptr)));
        int verificationCode = rand() % 9000 + 1000;
        if (!clif::input("To continue, you must rewrite the four-digit verification code and enter it. ["+ std::to_string(verificationCode) +"]: ") .compare(std::to_string(verificationCode)) == 0) {
            clif::log(INFO,"purge cancelled by force");
            return;
        }
        if (!clif::confirm("Are you sure you want to purge CLIBX?")) {
            clif::log(INFO,"purge cancelled by user");
            return;
        }
    }

    fs::path appPath = HOME / ".local" / "bin" / "clibx";
    if (!fs::exists(appPath)) {
        clif::log(FATAL, "CLIBX does not exist.",2);
    }

    try {
        if (fs::exists(HOME_DIRECTORY))fs::remove_all(HOME_DIRECTORY);
        clif::log(INFO,".clibx was removed");
        fs::remove(appPath);
        clif::log(INFO,"\033[32mthe program has been successfully purged, SAY GOODBYE\033[0m");

    } catch (const fs::filesystem_error& e) {
        clif::log(FATAL, "error removing CLIBX: " + std::string(e.what()),2);
    }
}
void report(const std::string title, const std::string body, const std::string assignee, const std::string labels){
    sysf({"xdg-open",stringf::escape("https://github.com/rudolfmuller/clibx-package-manager/issues/new?title=" + title + "&body=" + body +  "&assignee=" + assignee + "&labels=" + labels + "/"), "&>> /dev/null 2>&1"});
}
}