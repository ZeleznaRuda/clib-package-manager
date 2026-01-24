#include "../include/yamlf.h"

namespace yaml {

std::string read(const fs::path &fileName) {
  if (!fs::exists(fileName))
    clif::log(FATAL, "file " + fileName.string() + " not found", 1);

  std::ifstream file(fileName);
  if (!file.is_open())
    clif::log(FATAL, "cannot open file", 1);

  return std::string((std::istreambuf_iterator<char>(file)),
                     std::istreambuf_iterator<char>());
}

yaml_t parser(const std::string &fileContent) {
  std::unordered_map<std::string,
                     std::variant<std::string, std::vector<std::string>>>
      data;
  auto lines = stringf::split(fileContent, '\n');
  if (!lines.empty() && lines[0].rfind("# @", 0) == 0) {
    data["@"] = lines[0];
  }
  for (auto &l : lines) {
    l = stringf::strip(l);
    if (l.empty() || l[0] == '#')
      continue;

    auto hashPos = l.find('#');
    if (hashPos != std::string::npos)
      l = l.substr(0, hashPos);

    l = stringf::strip(l);
    if (l.empty())
      continue;

    auto parts = l.find('=');
    if (parts != std::string::npos) {
      std::string key = stringf::strip(l.substr(0, parts));
      std::string value = stringf::strip(l.substr(parts + 1));

      if (!value.empty() && value.front() == '(' && value.back() == ')') {
        value = stringf::strip(value.substr(1, value.size() - 2));
        auto items = stringf::split(value, ',');

        std::vector<std::string> vectorContent;

        for (auto &item : items) {
          item = stringf::strip(item);
          if (!item.empty() && item.front() == '"' && item.back() == '"') {
            item = item.substr(1, item.size() - 2);
          }
          if (!item.empty())
            vectorContent.push_back(item);
        }

        data[key] = vectorContent;
        continue;

      } else {
        if (!value.empty() && value.front() == '"' && value.back() == '"') {
          value = value.substr(1, value.size() - 2);
        }
        data[key] = value;
      }
    }
  }

  return data;
}

} // namespace yaml
