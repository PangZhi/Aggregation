// Copyright (c) 2014, Jing Fan. All Rights Reserved.

#include "aggregation/ranker/tfidf_ranker.h"

#include <algorithm>
#include <iostream>
#include <cmath>
#include <cstring>
#include <fstream>
#include <utility>
#include <unordered_map>
#include <unordered_set>

#include "aggregation/idf_gen.h"

namespace aggregation {

typedef std::pair<Token, Token> token_pair;

TfidfRanker::TfidfRanker(const std::string& corpus_path)
    : corpus_path_(corpus_path) {
  std::ifstream fin("idf.txt");
  // Generate idf.txt.
  /*
  if (!fin) {
    // std::cout << "ERROR: can't open idf.txt" << "\n";
    IdfGen idf_gen;
    idf_gen.GenIdf(corpus_path_.c_str(), "idf.txt", idf_map_);
    return;
  }
  */
  // Read idf.txt.
  char buf[1024];
  char str[1024];
  char weight_str[1024];
  double weight;
  while (!fin.eof()) {
    fin.getline(buf, 1024);
    sscanf(buf, "%[^|]|%s", str, weight_str);
    idf_map_[std::string(str)] = atof(weight_str);
  }
}


void TfidfRanker::Rank(const std::vector<Post>& posts,
                       const int knum,
                       const int backup_num,
                       Graph* graph,
                       Graph* backup_graph) {
  
  std::unordered_map<std::string, int> entity_cnt_map;
  for (Post p : posts) {
    for (auto iter = p.tokens.begin(); iter != p.tokens.end(); ++iter) {
      // TODO(fanj):Test whether split entity and event is useful.
      if (iter->token_type != TokenType::kEntity) {
        continue;
      }
      ++entity_cnt_map[iter->name];
    }
  }

  // calculate tfidf for entity words.
  // TODO(fanj): Add different functions for tfidf.
  std::vector<Node> node_vec;
  for (auto& item : entity_cnt_map) {
    if (idf_map_.find(item.first) != idf_map_.end()) {
      node_vec.push_back(std::move(Node(std::move(Token(item.first, TokenType::kEntity)),
      item.second,
      item.second * (log(idf_map_[item.first]) + 1))));
    } else {
      node_vec.push_back(Node(Token(item.first, TokenType::kEntity),
                              item.second,
                              item.second * (3 + 1)));
    }
  }

  // rank words by tfidf weight
  std::sort(node_vec.begin(), node_vec.end(), CompareByWeight);

  // TODO(fanj):Whether it's out of range?
  // Generate backup graph.
  int keep_num = std::min(knum /2, static_cast<int>(node_vec.size()));
  node_vec.erase(node_vec.begin() + keep_num, node_vec.end()); 
  for (const Node& node : node_vec) {
    backup_graph->AddNode(node); 
  }
  std::cout << "**** backup graph ****\n";
  backup_graph->OutputNodes();

  // Generate result graph.
  keep_num = std::min(knum, static_cast<int>(node_vec.size()));
  node_vec.erase(node_vec.begin() + keep_num, node_vec.end());
  for (const Node& node : node_vec) {
    graph->AddNode(node);
  }
  std::cout << "**** graph ****\n";
  graph->OutputNodes();

  // all the useful entity in this collection of posts
  // std::unordered_set all_entity_set(node_vec.begin(), node_vec.end());
  /**
  std::unordered_set<Token> all_entity_set;
  for (auto& node : node_vec) {
    all_entity_set.insert(node.token);
  }

  std::unordered_map<std::pair<Token, Token>, int>  co_occur_cnt_map;
  std::unordered_map<Token, std::unordered_set<Token> > co_occur_map;
  
  for (Post p : posts) {
    // useful entity in this post
    std::unordered_set<Token> entity_set;
    for (Token& t : p.tokens) {
      if (all_entity_set.find(t) != all_entity_set.end()) {
        entity_set.insert(t);
      }
    }
    
    for (auto& item : entity_set) {
      for (Token &t : p.tokens) {
        if (t.token_type != TokenType::kEvent) {
          continue;
        }
        co_occur_map[item].insert(t);
        ++co_occur_cnt_map[std::pair<Token, Token>(item, t)];
      }
    } 
  }

  // Generate a new graph
  for (Node& n : node_vec) {
    std::cout << n.token.name << " " << n.cnt << " " << n.weight << "\n";
    Token entity_token = n.token;
    node_id nid = graph->AddNode(std::move(n));  
    for (auto item : co_occur_map[n.token]) {
      // std::cout << item.name << " : " << co_occur_cnt_map[std::pair<Token, Token>(n.token, item)] << " | " ;
      node_id nid2 = graph->AddNode(item, 0, 0);
      int co_occur_cnt = co_occur_cnt_map[token_pair(entity_token, item)];
      // std::cout << "nid : " << nid << " | nid2 : " << nid2 << std::endl;  
      graph->AddEdge(nid, nid2, co_occur_cnt, static_cast<double>(co_occur_cnt));
    }
    // std::cout << "\n";
  }
  **/
}

void TfidfRanker::Rank(const std::vector<Graph*>& graphs,
                       const int knum,
                       const int backup_num,
                       Graph* graph,
                       Graph* backup_graph) {
  std::unordered_map<Token, double> token_weight_map;
  for (const Graph* g : graphs) {
    for (const Node& node : g->nodes()) {
      token_weight_map[node.token] += node.weight;
    }
  }
  
  std::vector<Node> node_vec;
  for (auto item : token_weight_map) {
    node_vec.push_back(Node(item.first, 0, item.second));
  }

  std::sort(node_vec.begin(), node_vec.end(), CompareByWeight);
  int keep_num = std::min(knum /2, static_cast<int>(node_vec.size()));
  node_vec.erase(node_vec.begin() + keep_num, node_vec.end()); 
  for (const Node& node : node_vec) {
    backup_graph->AddNode(node); 
  }
  std::cout << "**** backup graph ****\n";
  backup_graph->OutputNodes();

  // Generate result graph.
  keep_num = std::min(knum,  static_cast<int>(node_vec.size()));
  node_vec.erase(node_vec.begin() + keep_num, node_vec.end());
  for (const Node& node : node_vec) {
    graph->AddNode(node);
  }
  std::cout << "**** graph ****\n";
  graph->OutputNodes();  
}
} // namespace aggregation
