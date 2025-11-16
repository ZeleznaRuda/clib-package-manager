#include "../include/configf.h"
namespace configf{
std::string loadConfig(const std::string& key){
    if (fs::exists(core::homeDirectory)){ 
        return yaml::parser(yaml::read(core::homeDirectory / "_sys" / "_config.yaml"))[key];
    }
    return "";
}
void changeConfig(const std::string& key, const std::string& content) {
    yaml::changeKey((core::homeDirectory / "_sys" / "_config.yaml"), key, content);
    cli::log(INFO, "config has been changed");
}
}