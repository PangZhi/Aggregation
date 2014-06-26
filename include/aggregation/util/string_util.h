#ifndef STRING_UTIL_H_
#define STRING_UTIL_H_

#include <sstream>
#include <string>
#include <vector>

namespace util {

std::vector<std::string> Split(const std::string& s, const char delim);

} // namespace util

#endif

