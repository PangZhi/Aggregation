// Copyright (c) 2014, Jing Fan. All Rights Reserved.

#include "aggregation/util/option.h"

#include <assert.h>

#include <fstream>

#include <algorithm> 
#include <functional> 
#include <cctype>
#include <locale>

// trim from start
static inline std::string &ltrim(std::string &s) {
  s.erase(s.begin(), std::find_if(s.begin(), s.end(), 
          std::not1(std::ptr_fun<int, int>(std::isspace))));
  return s;
}

// trim from end
static inline std::string &rtrim(std::string &s) {
  s.erase(std::find_if(s.rbegin(), s.rend(),
          std::not1(std::ptr_fun<int,int>(std::isspace))).base(), s.end());
  return s;
}

// trim from both ends
static inline std::string &trim(std::string &s) {
  return ltrim(rtrim(s));
}

namespace util {

  std::string& Option::GetOption(const std::string& s) {
    return option_map_[s]; 
  }

  void Option::Parse() {
    std::ifstream fin("Config.txt");
    if (!fin) {
      std::cout << "ERROR: can't open Config.txt, use default settings\n";
      return;
    }
    const int kBufSize = 1024;
    char buf[kBufSize];
    char option_str[kBufSize];
    char option_setting[kBufSize];
    while (!fin.eof()) {
      fin.getline(buf, kBufSize);
      if ('\0' == buf[0]) {
        break;
      }
      sscanf(buf, "%[^:]:%s", option_str, option_setting);
      std::string str(option_str);
      str = trim(str);
      std::string setting(option_setting);
      setting = trim(setting);
      option_map_[str] = setting;
      std::cout << str << " : " << setting << "\n";
    }
    fin.close();
  }

  void Option::Parse(const int argc, const char** argv) {

  }

} // namespace util
