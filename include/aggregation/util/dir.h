// Copyright (c) 2014, Jing Fan. All Rights Reserved.

#ifndef UTIL_DIR_H_
#define UTIL_DIR_H_

#include <dirent.h>
#include <cstring>
#include <iostream>
#include <string>
#include <vector>

namespace util {

const int kBufSize = 1024;

/**
 * @brief Get all the children file names given a directory name.
 * @param dir_path The absoluate path of the directory.
 */
std::vector<std::string> GetFileNamesFromDir(const std::string& dir_path) {
  std::vector<std::string> file_names;
  DIR* dirp = nullptr;
  struct dirent* direntp = nullptr;
  dirp = opendir(dir_path.c_str());
  if (nullptr == dirp) {
    std::cout << "Warning: Can't open directory : " << dir_path << "\n";
    return file_names;
  }
  char buf[kBufSize];
  while ((direntp = readdir(dirp)) != nullptr) {
    if (strncmp(direntp->d_name , ".", 1) == 0) {
      continue;
    }
    std::cout << direntp->d_name << std::endl;
    strcpy(buf, dir_path.c_str());
    strcat(buf, "/");
    strcat(buf, direntp->d_name);
    std::cout << buf << std::endl;
    file_names.push_back(std::string(buf));
  }
  closedir(dirp);
  return file_names;
}

} // namespace util

#endif // UTIL_DIR_H_
