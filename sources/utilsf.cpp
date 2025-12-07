#include "../include/utilsf.h"

namespace utilsf
{


    std::string escapeShellArg(const std::string& arg) {
        std::string escaped = "'";
        for (char c : arg) {
            if (c == '\'')
                escaped += "'\\''";
            else
                escaped += c;
        }
        escaped += "'";
        return escaped;
    }

    bool ends_with(const std::string& str, const std::string& suffix) {
        return str.size() >= suffix.size() &&
               str.compare(str.size() - suffix.size(), suffix.size(), suffix) == 0;
    }

    bool start_with(const std::string& str, const std::string& prefix) {
        return str.size() >= prefix.size() &&
               str.compare(0, prefix.size(), prefix) == 0;
    }

    std::vector<std::string> split(const std::string& text, char delimiter) {
        std::vector<std::string> result;
        std::stringstream ss(text);
        std::string item;
        while (std::getline(ss, item, delimiter)) {
            result.push_back(item);
        }
        return result;
    }

    std::string strip(const std::string& s) {
        size_t start = 0, end = s.size();
        while (start < s.size() && std::isspace(static_cast<unsigned char>(s[start]))) ++start;
        while (end > start && std::isspace(static_cast<unsigned char>(s[end - 1]))) --end;
        return s.substr(start, end - start);
    }

    std::string join(const std::vector<std::string>& vec, const std::string& delim) {
        std::string result;
        for (size_t i = 0; i < vec.size(); ++i) {
            result += vec[i];
            if (i != vec.size() - 1) result += delim;
        }
        return result;
    }

}
