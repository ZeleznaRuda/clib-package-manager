#include "../include/configf.h"
namespace configf{

std::string loadConfig(const std::string& key){
    if (fs::exists(homeDirectory)){ 
        return yaml::parser(yaml::read(homeDirectory / "_sys" / "_config.yaml"))[key];
    }
    return "";
}
void changeConfig(const std::string& key, const std::string& content) {
    yaml::changeKey((homeDirectory / "_sys" / "_config.yaml"), key, content);
    clif::log(INFO, "config has been changed");
}
}