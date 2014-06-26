// Copyright (c) 2014, Jing Fan. All Rights Reserved.

#include "aggregation/ranker/jaccard_ranker.h"

#include <algorithm>
#include <iostream>
#include <unordered_set>

#include "aggregation/idf_gen.h"

namespace aggregation {

  int debug_index = -1;
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

struct PageRankEdge {
  PageRankEdge(const int o_index) : index(o_index), weight(0.15) {}
  PageRankEdge(const int o_index, const double o_weight)
      : index(o_index), weight(o_weight) {}
  int index;
  double weight;
};


void PageRank(std::vector<Vertex>& vertices, std::vector<std::vector<PageRankEdge> >& edges, const int max_iter = 10) {
  int size = vertices.size();
  std::vector<double> sum_weight(size);
  std::vector<Vertex> new_vertices(vertices);
  // Calculate sum of weight for every vertex.
  for (int i = 0; i < size; ++i) {
    for (auto& e : edges[i]) {
      sum_weight[i] += e.weight;
    }
  }
  
  int loop_cnt = 0;
  while (loop_cnt < max_iter) {
    for (int i = 0; i < size; ++i) {
      double sum = 0;
      for (auto& e : edges[i]) {
        if (sum_weight[e.index] != 0) {
          sum += vertices[e.index].score * e.weight / sum_weight[e.index]; 
        }
      }
      new_vertices[i].score = 0.15 + 0.85 * sum;
      new_vertices.swap(vertices);
    }
    ++loop_cnt;
  }
  std::sort(vertices.begin(), vertices.end());
}

void JaccardRanker::Rank(const std::vector<Post>& posts,
                         const int knum,
                         const int backup_num,
                         Graph* graph,
                         Graph* backup_graph) {
  std::vector<Node> node_vec;
  std::vector<std::vector<PageRankEdge> > edges;
  std::vector<std::unordered_set<int> > postids;
  std::unordered_map<Token, int> token2index_map;
  std::unordered_set<Token> tokens;
  std::unordered_map<Token, int> token_cnt_map;
  for (const Post& p : posts) {
    for (const Token& t : p.tokens) {
      ++token_cnt_map[t];
    }
  }
  int post_index = 0;
  for (const Post& p : posts) {
    for (const Token& t : p.tokens) {
      if (tokens.find(t) == tokens.end()) {
        if (token_cnt_map[t] <=2) continue;
        node_vec.push_back(Node(t,1,0));
        if (t.name == "minnesota") {
std::cout << "the index is : " << node_vec.size() - 1;
          debug_index = node_vec.size() - 1;
        } 
        token2index_map[t] = node_vec.size() - 1;
        edges.push_back(std::vector<PageRankEdge>());
        postids.push_back(std::unordered_set<int>());
        postids[node_vec.size()-1].insert(post_index);
        tokens.insert(t);
      } else {
        int index = token2index_map[t];
        ++node_vec[index].cnt; 
        postids[index].insert(post_index);
      }
    }
    ++post_index;
  }                

  typedef std::pair<int, int> id_pair;
  std::unordered_map<id_pair, int> union_map;
  std::unordered_map<id_pair, int> intersect_map;
  // std::vector<std::unordered_set<int> > postids(node_vec.size());
  int size = node_vec.size();
  for (int i = 0 ; i < size; ++i) {
    for (int j = i + 1; j < size; ++j) {
      std::unordered_set<int> union_postids(postids[i]);
      union_postids.insert(postids[j].begin(), postids[j].end());
      int union_size = union_postids.size();

      union_map[id_pair(i,j)] = union_size;
      union_map[id_pair(j,i)] = union_size;

      int intersect_size = 0;
      // TODO(fanj): Use math formula to similify calculation.
      for (int id : postids[j]) {
        if (postids[i].find(id) != postids[i].end()) {
          ++intersect_size;
        }
      }
      intersect_map[id_pair(i, j)] = intersect_size;
      intersect_map[id_pair(j, i)] = intersect_size;

      double jaccard_similarity = static_cast<double>(intersect_size) / union_size;
 
      if (jaccard_similarity < 0.1) {
        continue;
      }
      edges[i].push_back(PageRankEdge(j, jaccard_similarity));
      edges[j].push_back(PageRankEdge(i, jaccard_similarity));
    }
  }
  std::vector<Vertex> vertices;
  int pos = 0;
  for (const Node& node : node_vec) {
    vertices.push_back(Vertex(pos, 0.15));
    ++pos;
  }

  PageRank(vertices, edges);
  int keep_num = std::min(knum * 10, static_cast<int>(vertices.size()));
  vertices.erase(vertices.begin() + keep_num, vertices.end());
  for (auto v : vertices) {
    // std::cout << node_vec[v.index].token.name << " " << v.score << " | ";
  }
  std::cout << "\n";
  for (const Vertex& v : vertices) { 
    node_id id = backup_graph->AddNode(node_vec[v.index].token, 0, 0);
    for (const PageRankEdge& e : edges[v.index]) {
      node_id id2 = backup_graph->AddNode(node_vec[e.index].token, 0, 0);
      backup_graph->AddEdge(id, id2, 0, 0);
    }
  }

  keep_num = std::min(knum, static_cast<int>(vertices.size()));
  vertices.erase(vertices.begin() + keep_num, vertices.end());

  for (const Vertex& v : vertices) {
    std::cout << v.index << " ";
    graph->AddNode(node_vec[v.index].token, 0, v.score);
  }
  std::cout << "\n";
  graph->OutputNodes();
}


void JaccardRanker::Rank(const std::vector<Graph*>& graphs,
                         const int knum,
                         const int backup_num,
                         Graph* graph,
                         Graph* backup_graph) {

}


} // namespace aggregation
