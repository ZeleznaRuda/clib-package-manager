#include "../include/appf.h"
namespace appf {
void purge(const bool force) { // apk-uninstall
    if (!force) {
        if (!clif::confirm("Are you sure you want to purge CLIBX?")) {
            clif::log(INFO,"purge cancelled by user");
            return;
        }
    }

    fs::path Path = userHomeDirectory / ".local" / "bin" / "clibx";

    if (!fs::exists(Path)) {
        clif::log(FATAL, "CLIBX does not exist.",2);
    }

    try {
        if (fs::exists(homeDirectory)) fs::remove_all(homeDirectory);
        clif::log(INFO,".clibx was removed");
        if (fs::exists(Path)) fs::remove(Path);
            clif::log(INFO,"\033[32mthe program has been successfully purged, SAY GOODBYE\033[0m");
    } catch (const fs::filesystem_error& e) {
        clif::log(FATAL, "error removing CLIBX: " + std::string(e.what()),2);
    }
}
void report(const std::string title, const std::string body, const std::string assignee, const std::string labels){
    std::system(std::string("xdg-open \"https://github.com/rudolfmuller/clibx-package-manager/issues/new?title=" + title + "&body=" + body +  "&assignee=" + assignee + "&labels=" + labels + "\" &>> /dev/null 2>&1").c_str());
    std::exit(0);
}
}