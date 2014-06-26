// Copyright (c) 2014, Jing Fan. All Rights Reserved.

#include "aggregation/ranker/submodular_ranker.h"

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

SubmodularRanker::SubmodularRanker(const std::string& corpus_path)
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


struct Vertex {
  Vertex(const int o_index) : index(o_index), score(0) {}
  Vertex(const int o_index, const double o_score)
      : index(o_index), score(o_score) {}
  bool operator < (const Vertex& v) const {
    return score > v.score;
  } 
  int index;
  double score;
  
};


void SubmodularRanker::Rank(const std::vector<Post>& posts,
                       const int knum,
                       const int backup_num,
                       Graph* graph,
                       Graph* backup_graph) {

  std::vector<Node> node_vec;
  std::unordered_map<Token, int> token_cnt_map;

  for (const Post& p : posts) {
    for (const Token& t : p.tokens) {
      ++token_cnt_map[t];
    }
  }

  for (const auto& item : token_cnt_map) {
    // TODO(fanj): What if there is no corresponding word in idf_map_?
    node_vec.push_back(Node(item.first, item.second,
      static_cast<double>(item.second) * (log(idf_map_[item.first.name])+ 1 )));
  }
  // Sort node so that they have descending weight.
  std::sort(node_vec.begin(), node_vec.end(), CompareByWeight);
  // Filter low cnt and weight ones.
  int keep_num = std::min(knum * 10, static_cast<int>(node_vec.size()));
  node_vec.erase(node_vec.begin() + keep_num, node_vec.end());
  graph->AddNodes(node_vec.begin(), node_vec.end());
  
  // Keep the information of each node(token) occur in which posts. The index  
  // is the node id of a token, the unordered_set is the post ids which this
  // token has occurred.
  std::vector<std::unordered_set<int> > postids(node_vec.size());
  
  // Iterate over all posts and generate nodeid2postid_map_.
  int cnt = 0;
  for (const Post& p : posts) {
    std::unordered_set<Token> tokens(p.tokens.begin(), p.tokens.end());
    for (const Token& t : tokens) {
      int index = graph->GetNodeIdByToken(t);
      if (index == -1) {
        continue;
      } else {
        postids[index].insert(cnt);
      }
    }
    ++cnt;
  }

  int size = node_vec.size();
  double sim[size][size];
  int union_array[size][size];
  for (int i = 0; i < size; ++i) {
    for (int j = 0; j < size; ++j) {
      sim[i][j] = 0;
      union_array[i][j] = 0;
    }
  }

  for (int i = 0; i < size; ++i) {
    for (int j = i + 1; j < size; ++j) {
      // Calculate jaccard similarity for node i and node j.
      std::unordered_set<int> union_postids(postids[i].begin(), postids[i].end());
      union_postids.insert(postids[j].begin(), postids[j].end());
      int union_size = union_postids.size();
      int intersect_size = 0;
      for (int id : postids[j]) {
        if (postids[i].find(id) != postids[i].end()) {
          ++intersect_size;
        }
      }
      union_array[i][j] = union_array[j][i] = intersect_size;
      double jaccard_similarity = 0;
      if (union_size != 0 ) {
        if (intersect_size <= 2) jaccard_similarity = 0;
        else 
        jaccard_similarity = static_cast<double>(intersect_size) / union_size;
      }
      // jaccard_similarity = sqrt(jaccard_similarity);
      // TODO(fanj): Change the threhold.
      if (jaccard_similarity > 0.01) {
        std::cout << "submodularity is : " << jaccard_similarity << "\n";
        graph->AddEdge(i, j, 0, jaccard_similarity);
        graph->AddEdge(j, i, 0, jaccard_similarity);
        sim[i][j] = sim[j][i] = jaccard_similarity;
      }
    }
  }

  // Use submular function to choose knum most important words. We should care
  // both the tfidf rank and diversity coverage.

  for (const Node& node : node_vec) {
    std::cout << node.token.name << " " << node.weight << " | ";
  }
  std::cout << "\n";

  std::vector<Vertex> submodular_ret;
  // Node& new_added_node;
  std::vector<Vertex> scores;
  int pos = 0;
  for (const Node& node : node_vec) {
    scores.push_back(Vertex(pos, node.weight));
    ++pos;
  }
  int word_cnt = 0;
  // TODO(fanj):Use faster algorithm.
  std::sort(scores.begin(), scores.end());
  submodular_ret.push_back(scores[0]);
  scores.erase(scores.begin());
  ++word_cnt;
  while (word_cnt < knum) {
    Vertex& v = submodular_ret.back();
    for (Vertex& ver : scores) {
      // ver.score = ver.score * (1 - sqrt(sim[v.index][ver.index]));
      ver.score -= ver.score * (static_cast<double>(union_array[ver.index][v.index])) / node_vec[ver.index].cnt;
      if (ver.score < 0) {
        ver.score = 0;
      }
    }
    std::sort(scores.begin(), scores.end());
    submodular_ret.push_back(scores[0]);
    scores.erase(scores.begin());
    ++word_cnt;
  }
   
  for (const Vertex& v : submodular_ret) {
    std::cout << node_vec[v.index].token.name << " " << v.score << " | ";
  }
  std::cout << std::endl;

}

void SubmodularRanker::Rank(const std::vector<Graph*>& graphs,
                       const int knum,
                       const int backup_num,
                       Graph* graph,
                       Graph* backup_graph) {


}
} // namespace aggregation
