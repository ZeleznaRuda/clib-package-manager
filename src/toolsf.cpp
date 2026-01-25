#include "../include/toolsf.h"

sysf_t sysf(const std::vector<std::string> &argv) {
  std::string command;
  std::string output;
  for (const std::string &arg : argv) {
    (stringf::starts_with(arg, "\"") && stringf::ends_with(arg, "\""))
        ? command += arg
        : command += arg + " ";
  }
  fs::path logPath = HOME_DIRECTORY / "_sys" / "logs" /
                     (std::to_string(std::time(nullptr)) + ".log");
  int result = std::system((command + "&>" + logPath.string()).c_str());
  if (DEBUG_MODE)
    clif::log(DEBUG, command);
  std::ifstream log(logPath);
  if (log) {
    output = std::string((std::istreambuf_iterator<char>(log)),
                         std::istreambuf_iterator<char>());
  }

  return {result, output};
}

namespace stringf {
std::string escape(const std::string &arg) {
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

bool starts_with(const std::string &str, const std::string &prefix) {
  return str.size() >= prefix.size() &&
         str.compare(0, prefix.size(), prefix) == 0;
}

bool ends_with(const std::string &str, const std::string &suffix) {
  return str.size() >= suffix.size() &&
         str.compare(str.size() - suffix.size(), suffix.size(), suffix) == 0;
}

std::vector<std::string> split(const std::string &text, char delimiter) {
  std::vector<std::string> result;
  std::stringstream ss(text);
  std::string item;
  while (std::getline(ss, item, delimiter)) {
    result.push_back(item);
  }
  return result;
}

std::string strip(const std::string &s) {
  size_t start = 0, end = s.size();
  while (start < s.size() && std::isspace(static_cast<unsigned char>(s[start])))
    ++start;
  while (end > start && std::isspace(static_cast<unsigned char>(s[end - 1])))
    --end;
  return s.substr(start, end - start);
}

std::string join(const std::vector<std::string> &vec,
                 const std::string &delim) {
  std::string result;
  for (size_t i = 0; i < vec.size(); ++i) {
    result += vec[i];
    if (i != vec.size() - 1)
      result += delim;
  }
  return result;
}
} // namespace stringf
