// Copyright (c) 2014, Jing Fan. All Rights Reserved.

#include <dirent.h>
#include <cstdlib>
#include <cstring>
#include <fstream>
#include <iostream>

#include "aggregation/util/preprocess.h"

const int kBufSize = 4096;

int main(int argc, char** argv) {
  const char* input_dir = nullptr;
  const char* output_dir = nullptr;

  if (argc == 2) {
    input_dir = argv[0];
    output_dir = argv[1];
  } else {
    input_dir = "/u/f/a/fanj/dataset/arXiv/metadata-2013-txt";
    output_dir = "/u/f/a/fanj/dataset/arXiv/metadata-2013-processed";
  }

  aggregation::util::Preprocess p;
  
  DIR* dirp = nullptr;
  struct dirent* direntp = nullptr;
  dirp = opendir(input_dir);
  if (nullptr == dirp) {
    std::cout << "ERROR: Can't open data directory\n";
    exit(-1);
  }

  while ((direntp = readdir(dirp)) != nullptr) {
    if (strncmp(direntp->d_name, ".", 1) == 0) {
      continue;
    }
    std::cout << direntp->d_name << std::endl;
    
    char buf[kBufSize];
    strcpy(buf, input_dir);
    strcat(buf, "/");
    strcat(buf, direntp->d_name);
    std::cout << "Note : Input file is : " << buf << std::endl;
    std::ifstream fin(buf);
    // TODO(fanj): Check whether is correct.
    if (!fin) {
      std::cout << "ERROR: Can't open file " << buf << "\n";
      continue;
    }
    
    strcpy(buf, output_dir);
    strcat(buf, "/");
    strcat(buf, direntp->d_name);
    std::cout << "Note : Output file is : " << buf << std::endl;
    std::ofstream fout(buf);

    while (!fin.eof()) {
      std::vector<std::string> strs;
      char buf[kBufSize];
      fin.getline(buf, kBufSize, '\t');
      strs.push_back(std::string(buf));
      fout << buf << "\t";
      fin.getline(buf, kBufSize, '\t');
      std::string areas(buf);
      // std::vector<std::string> ss = std::Split(areas, ';');

      strs.push_back(areas);
      fout << buf << "\t";

      fin.getline(buf, kBufSize);
      std::string text(buf);
      // strs.push_back(text);
      std::vector<std::string> tokens;
      p.Process(text.c_str(), tokens);
      for (std::string& s : tokens) {
        fout << s << " ";
      }
      fout << "\b" << std::endl;
    }
    fin.close();
    fout.close();


  }
}
