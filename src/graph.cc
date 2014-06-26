// Copyright (c) 2014, Jing Fan. All Rights Reserved.

#include "aggregation/graph.h"

#include <assert.h>
#include <iostream>

namespace  aggregation {

node_id Graph::AddNode(const aggregation::Token& token, int cnt, double weight) {
  if (token2id_map_.find(token) != token2id_map_.end()) {
    return token2id_map_[token];
  }
  nodes_.push_back(std::move(Node(token, cnt, weight)));
  token2id_map_[token] = nodes_.size() - 1;
  return (nodes_.size() - 1);
}

node_id Graph::AddNode(const Node& n) {
  if (token2id_map_.find(n.token) != token2id_map_.end()) {
    return token2id_map_[n.token];
  }
  nodes_.push_back(n);
  token2id_map_[n.token] = nodes_.size() - 1;
  return nodes_.size() - 1;
}

void Graph::AddNodes(std::vector<Node>::iterator first, std::vector<Node>::iterator last) {
  std::vector<Node>::iterator iter = first;
  while (iter != last) {
    AddNode(std::move(*iter));
    ++iter;
  }            
}

void Graph::AddEdge(node_id main_nid, node_id nid, int cnt, double weight) {
  assert(main_nid >= 0);
  assert(main_nid < nodes_.size());
  nodes_[main_nid].edges.push_back(Edge(nid, cnt, weight));
}

void Graph::OutputToFile(const std::string& filepath) {
  
}

void Graph::OutputNodes() const {
  for (const Node& node : nodes_) {
    std::cout << node.token.name << ":" << node.weight << "\n";
  }
  std::cout << "\n";
}

} // namespace aggregation
