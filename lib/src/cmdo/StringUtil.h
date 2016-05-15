#ifndef CMDO_STRINGUTIL_H
#define CMDO_STRINGUTIL_H

#include <string>
#include <sstream>
#include <exception>
#include <array>
#include <algorithm>


namespace cmdo {

struct BadCast : public std::exception {

};
std::array<std::string, 2> const TRUE_STRINGS = {{"true", "yes"}};
std::array<std::string, 2> const FALSE_STRINGS = {{"false", "no"}};

template<typename T>
static std::string to_string(const T &v);

template<>
std::string to_string<bool>(const bool &v) {
  if (v) {
    return TRUE_STRINGS[0];
  } else {
    return FALSE_STRINGS[0];
  }
}

template<typename T>
std::string to_string(const T &v) {
  std::stringstream ss;
  ss << v;
  return ss.str();
}

template<typename T>
static T from_string(std::string const &str);

template<>
bool from_string<bool>(std::string const &str) {
  std::string str_(str);
  for (char &c: str_) {
    c = std::tolower(c);
  }

  if (std::any_of(TRUE_STRINGS.begin(),
                  TRUE_STRINGS.end(),
                  [str_](std::string const &s) { return s == str_; })) {
    return true;
  }
  if (std::any_of(FALSE_STRINGS.begin(),
                  FALSE_STRINGS.end(),
                  [str_](std::string const &s) { return s == str_; })) {
    return false;
  }
  throw BadCast();
}

template<typename T>
T from_string(std::string const &str) {
  T v;
  std::stringstream ss;
  ss.str(str);
  ss >> v;
  if (ss.fail()) {
    throw BadCast();
  }
  return v;
}

inline
void trim(std::string &flag) {
  std::string result;
  for (size_t i(0); i < flag.size(); ++i) {
    if (flag[i] == ' ') {
      continue;
    }
    result = flag.substr(i);
    break;
  }
  if (!result.empty()) {
    size_t i(result.size() - 1);
    while (true) {
      if (result[i] != ' ') {
        result = result.substr(0, i + 1);
        break;
      }
      if (i == 0) {
        break;
      }
      i--;
    }
  }
  flag = result;
}

template<typename T>
static void split(std::vector<T> &result, std::string const &input,
                  char separator) {
  auto has_data = [](std::stringstream & ss) -> bool {
    ss.seekp(0, ss.end);
    return ss.tellp() != 0;
  };
  result.clear();

  std::stringstream ss;
  for(char const& c : input) {
    if(c == separator) {
      if(has_data(ss)) {
        result.push_back(ss.str());
        ss.str("");
        continue;
      } else {
        continue;
      }
    }
    ss << c;
  }
  if(has_data(ss)) {
    result.push_back(ss.str());
  }
}

}

#endif //CMDO_STRINGUTIL_H
