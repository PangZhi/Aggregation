// Copyright (c) 2014, Jing Fan. All Rights Reserved.

#include "aggregation/util/string_util.h"

namespace util {

  std::vector<std::string> Split(const std::string& s, const char delim) {
  std::vector<std::string> elems;
  std::string item;
  std::stringstream ss(s);
  while (std::getline(ss, item, delim)) {
    elems.push_back(item);
  }
  return elems;
}


} // namespace util
