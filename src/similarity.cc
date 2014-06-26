// Copyright (c) 2014, Jing Fan. All Rights Reserved.

#include "aggregation/util/similarity.h"

#include <algorithm>
#include <cmath>
#include <fstream>
#include <list>
#include <set>
#include <iostream>
#include <string>
#include <unordered_map>
#include <unordered_set>
#include <utility>

namespace util {

const double kThrehold = 0.5;

Similarity::Similarity() {
  option_.Parse();
}

typedef std::pair<std::string, double> node;

bool CompareByString(const node& lhs, const node& rhs) {
  return lhs.first < rhs.first;
}


void GetKGramOfString(const std::string& str, const int k, std::multiset<std::string> kgramset) {
  if (k > str.length()) {
    return;
  }
  for (auto liter = str.begin(); liter != str.end(); ++liter) {
    if (liter + k == str.end()) {
      break;
    }
    kgramset.insert(std::string(liter, liter+k));
  }

}
double jaccard_similarity(const std::string& lhs, const std::string& rhs, const int k) {
  std::multiset<std::string> kgramset1;
  std::multiset<std::string> kgramset2;

  GetKGramOfString(lhs, k, kgramset1);
  GetKGramOfString(rhs, k, kgramset2);

  std::multiset<std::string> union_gram(kgramset1.begin(), kgramset2.end());
  union_gram.insert(kgramset2.begin(), kgramset2.end());
  int intersect_size = 0;
  for (auto item : kgramset2) {
    if (kgramset1.find(item) != kgramset1.end()) {
      ++intersect_size;
      kgramset1.erase(item);
      kgramset2.erase(item);
    }
  }
  return 0;
}
double Similarity::GetSimilarity(const std::string& file1, const std::string& file2) {
  if (option_.GetOption("RankMethod") == "Tfidf") {
    
    std::vector<node> nodes1;
    std::vector<node> nodes2;
    
    std::unordered_map<std::string, int> string2index1;
    std::unordered_map<std::string, int> string2index2;

    std::unordered_set<std::string> string_set1;
    std::unordered_set<std::string> string_set2;

    std::unordered_map<int, int> mapping;
    
    std::ifstream fin(file1);
    int pos = 0;
    while (!fin.eof()) {
      node new_node;
      fin >> new_node.first >> new_node.second;
      string2index1[new_node.first] = pos++;
      string_set1.insert(new_node.first);
      nodes1.push_back(new_node);
    }
    fin.close();
    fin.open(file2);
    pos = 0;
    while (!fin.eof()) {
      node new_node;
      fin >> new_node.first >> new_node.second;
      if (string_set1.find(new_node.first) != string_set1.end()) {
        mapping[string2index1[new_node.first]] = pos;
        string_set1.erase(new_node.first);
      } else {
        string_set2.insert(new_node.first);
      }
      string2index2[new_node.first] = pos++;
      nodes2.push_back(std::move(new_node));
    }

    // TODO(fanj): Check whether the usage is correct here.
    for (auto str1 : string_set1) {
      for (auto str2 : string_set2) {
        if (jaccard_similarity(str1, str2, 2) < kThrehold) {
          string_set1.erase(str1);
          string_set2.erase(str2);
          mapping[string2index1[str1]] = string2index2[str2];
        }
      }
    }

    int match_size = mapping.size();
    int total_size = nodes1.size();
    double error_sum = 0;

    for (auto item : mapping) {
      node& node1 = nodes1[item.first];
      node& node2 = nodes2[item.second];
      error_sum += std::abs(node1.second - node2.second) / std::max(node1.second, node2.second);
    }
    error_sum += total_size - match_size;
    error_sum /= total_size;
    return 1 - error_sum;

  } else {
    std::cout << "ERROR: Can't deal with this rank method\n";
  } 
  return 0;
}

} // namespace util
