// Copyright (c) 2014, Jing Fan. All Rights Reserved.

#include "aggregation/arxiv_reader.h"

#include <assert.h>
#include <algorithm>
#include <cstdio>
#include <cstring>
#include <fstream>
#include <iostream>
#include <unordered_map>
#include <unordered_set>

#include "aggregation/meta_data.h"
#include "aggregation/util/dir.h"
#include "aggregation/util/string_util.h"

namespace aggregation {

void ArxivReader::Read(std::vector<Post>& posts,
    MetaData& metadata) {
  std::vector<std::string> file_names = std::move(
      util::GetFileNamesFromDir(data_path_)); 
  const int kBufSize = 4096;
  char buf[kBufSize];
  int cnt = 0;
  for (const std::string& file_name : file_names)  {
    std::ifstream fin(file_name);
    if (!fin) {
      std::cout << "ERROR: can't open file " << file_name << "\n";
      continue;
    }


    while (fin.getline(buf, kBufSize)) {
      std::string s(buf);
      std::vector<std::string> items = std::move(util::Split(s, '\t'));
      assert(items.size() == 3);
      std::vector<std::string> areas = std::move(util::Split(items[1], ';'));
      if (areas.size() == 0) {
        continue;
      }
      std::vector<std::string> area_pair = std::move(util::Split(areas[0], ':'));
      std::vector<std::string> token_strs = std::move(util::Split(items[2], ' '));
      std::vector<Token> tokens;

      for (const std::string& token_str : token_strs) {
        tokens.push_back(Token(token_str, TokenType::kEntity)); 
      }
      std::vector<std::string> data_vec;
      data_vec.push_back(items[0]);
      // Area
      data_vec.push_back(area_pair[0]);
      // SubArea
      if (area_pair.size() == 2) {
        data_vec.push_back(area_pair[1]);
      } else {
        data_vec.push_back("None");
      }
      metadata.AddMetaData(data_vec);

      posts.push_back(Post(std::move(data_vec),
          std::move(tokens)));
      cnt ++;
      if (cnt % 10000 == 0) {
        std::cout << "Read post " << cnt << "\n";
      } 
    }
    fin.close();
  }
  /*for (const auto& p : posts) {
    p.Print(); 
  }
  */
}

} // namespace aggregation
