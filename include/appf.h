#pragma once

#include <filesystem>
#include <fstream>

#include "../include/clif.h"
#include "../include/constants.h"
#include "../include/parsers.h"


namespace fs = std::filesystem;

namespace appf {
    void purge(const bool force = false);
}