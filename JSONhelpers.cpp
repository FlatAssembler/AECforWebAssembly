/*
 * JSON - JavaScript Object Notation
 * My compiler will output logging in that format, which is easy to read and
 * there is a lot of software for analyzing it.
 */

std::string JSONifyString(const std::string &str) {
  std::string ret = "\"";
  for (size_t i = 0; i < str.length(); i++)
    if (str[i] == '\n')
      ret += "\\n";
    else if (str[i] == '\"')
      ret += "\\\"";
    else if (str[i] == '\'')
      ret += "\\'";
    else if (str[i] == '\t')
      ret += "\\t";
    else if (str[i] == '\\')
      ret += "\\\\";
    else
      ret += str[i];
  ret += "\"";
  return ret;
}

std::string JSONifyVectorOfStrings(const std::vector<std::string> &vector) {
  if (vector.empty())
    return "[]";
  std::string ret = "[";
  for (size_t i = 0; i < vector.size(); i++)
    if (i == vector.size() - 1)
      ret += JSONifyString(vector[i]) + "]";
    else
      ret += JSONifyString(vector[i]) + ",";
  return ret;
}

std::string JSONifyVectorOfDoubles(const std::vector<double> &vector) {
  if (vector.empty())
    return "[]";
  std::string ret = "[";
  for (size_t i = 0; i < vector.size(); i++)
    if (i == vector.size() - 1)
      ret += std::to_string(vector[i]) + "]";
    else
      ret += std::to_string(vector[i]) + ",";
  return ret;
}

std::string JSONifyMapOfStrings(const std::map<std::string, std::string> &map) {
  std::vector<std::pair<std::string, std::string>> vector(map.size());
  std::copy(map.begin(), map.end(), vector.begin());
  std::string ret = "[\n";
  for (size_t i = 0; i < vector.size(); i++) {
    ret += "{\n\"first\":" + JSONifyString(vector[i].first) +
           ",\n\"second\":" + JSONifyString(vector[i].second) + "\n}";
    if (i != vector.size() - 1)
      ret += ",\n";
    else
      ret += "\n";
  }
  ret += "]";
  return ret;
}

std::string JSONifyMapOfUnsigneds(const std::map<std::string, unsigned> &map) {
  std::vector<std::pair<std::string, unsigned>> vector(map.size());
  std::copy(map.begin(), map.end(), vector.begin());
  std::string ret = "[\n";
  for (size_t i = 0; i < vector.size(); i++) {
    ret += "{\n\"first\":" + JSONifyString(vector[i].first) +
           ",\n\"second\":" + std::to_string(vector[i].second) + "\n}";
    if (i != vector.size() - 1)
      ret += ",\n";
    else
      ret += "\n";
  }
  ret += "]";
  return ret;
}

std::string JSONifyMapOfDoubles(const std::map<std::string, double> &map) {
  std::vector<std::pair<std::string, double>> vector(map.size());
  std::copy(map.begin(), map.end(), vector.begin());
  std::string ret = "[\n";
  for (size_t i = 0; i < vector.size(); i++) {
    ret += "{\n\"first\":" + JSONifyString(vector[i].first) +
           ",\n\"second\":" + std::to_string(vector[i].second) + "\n}";
    if (i != vector.size() - 1)
      ret += ",\n";
    else
      ret += "\n";
  }
  ret += "]";
  return ret;
}

std::string JSONifyMapOfInts(const std::map<std::string, int> &map) {
  std::vector<std::pair<std::string, int>> vector(map.size());
  std::copy(map.begin(), map.end(), vector.begin());
  std::string ret = "[\n";
  for (size_t i = 0; i < vector.size(); i++) {
    ret += "{\n\"first\":" + JSONifyString(vector[i].first) +
           ",\n\"second\":" + std::to_string(vector[i].second) + "\n}";
    if (i != vector.size() - 1)
      ret += ",\n";
    else
      ret += "\n";
  }
  ret += "]";
  return ret;
}
