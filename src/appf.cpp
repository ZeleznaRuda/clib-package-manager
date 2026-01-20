#include "../include/appf.h"
namespace appf {
void purge(const bool force) { // apk-uninstall
  if (!force) {
    std::srand(static_cast<unsigned int>(std::time(nullptr)));
    int verificationCode = rand() % 9000 + 1000;
    if (!clif::input("To continue, you must rewrite the four-digit "
                     "verification code and enter it. [" +
                     std::to_string(verificationCode) + "]: ")
             .compare(std::to_string(verificationCode)) == 0) {
      clif::log(INFO, "purge cancelled by force");
      return;
    }
    if (!clif::confirm("Are you sure you want to purge CCLM?")) {
      clif::log(INFO, "purge cancelled by user");
      return;
    }
  }

  /*fs::path appPath = HOME / ".local" / "bin" / "cclm";
  if (!fs::exists(appPath)) {
      clif::log(FATAL, "CCLM does not exist.",2);
  }*/

  try {
    if (fs::exists(HOME_DIRECTORY))
      fs::remove_all(HOME_DIRECTORY);
    clif::log(INFO, ".cclm was removed");
    // fs::remove(appPath);
    clif::log(
        INFO,
        "\033[32mthe program has been successfully purged, SAY GOODBYE\033[0m");

  } catch (const fs::filesystem_error &e) {
    clif::log(FATAL, "error removing CCLM: " + std::string(e.what()), 2);
  }
}
} // namespace appf
