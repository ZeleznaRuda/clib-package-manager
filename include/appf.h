#pragma once

#include <filesystem>
#include <fstream>

#include "../include/clif.h"
#include "../include/templates.h"
#include "../include/constants.h"
#include "../include/parsers.h"
#include "../include/configf.h"


namespace fs = std::filesystem;

namespace appf {
    void app_init();
    void app_clean(const bool force = false);
    void app_uninstall(const bool force = false);
    void app_config(const std::string& key, const std::string& content, const bool force);
}