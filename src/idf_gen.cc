// Copyright (c) 2014, Jing Fan. All Rights Reserved.

#include "aggregation/idf_gen.h"

#include <fstream>
#include <iostream>
#include <memory>
#include <unordered_map>
#include <unordered_set>

#include "aggregation/reader.h"
#include "aggregation/post_reader.h"
#include "aggregation/arxiv_reader.h"
#include "aggregation/meta_data.h"
#include "aggregation/util/option.h"

namespace aggregation {
  
void IdfGen::GenIdf(util::Option& option,
                    std::unordered_map<std::string, double>& idf_map) {
  
  Reader* reader;
  if ("Twitter" == option.GetOption("Type")) {
    reader = new PostReader(option.GetOption("DataDirectory"));
  } else if ("Arxiv" == option.GetOption("Type")){
    reader = new ArxivReader(option.GetOption("DataDirectory"));
  } else {
    std::cout << "ERROR: Unknown type\n";
    exit(-1);
  }
  
  std::vector<Post> posts;
  MetaData metadata(option.GetOption("Schema"), option.GetOption("Hierarchy"));
  reader->Read(posts, metadata);
  int post_num = posts.size();
  std::unordered_map<std::string, int> word_cnt_map;
  for (Post &p : posts) {
    for (Token &t : p.tokens) {
      ++word_cnt_map[t.name];
    }
  }
  std::string output_path;
  if ("" == option.GetOption("Idf")) {
    output_path = "idf.txt";
  } else {
    output_path = option.GetOption("Idf");
  }
                      std::cout << "Output idf to : " << output_path << std::endl;
  std::ofstream fout(output_path);
  for (auto iter = word_cnt_map.begin(); iter != word_cnt_map.end(); ) {
    if (iter->second <= 2) {
      iter = word_cnt_map.erase(iter);
      continue;
    }
    idf_map[iter->first] = static_cast<double>(post_num) / iter->second;
    fout << iter->first << "|" << idf_map[iter->first] << "\n";
    ++iter;
  }
  fout.close();
}

} // namespace aggregation
