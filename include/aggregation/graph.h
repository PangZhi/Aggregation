// Copyright (c) 2014, Jing Fan. All Rights Reserved.

#ifndef AGGREGATION_GRAPH_H_
#define AGGREGATION_GRAPH_H_

#include <string>
#include <utility>
#include <vector>
#include <unordered_map>

#include "aggregation/token.h"

namespace aggregation {

/**
 * @brief Definition of basic node, edge and graph stucture, which can be used
 *        to generate word cloud and used for higher level aggregation.
 */

/**
 * @brief Definition of node id.
 */
typedef int node_id;

/**
 * @brief Definition of undirected edge.
 */
struct Edge {
  Edge(node_id o_nid, int o_cnt, double o_weight)
      : nid(o_nid), cnt(o_cnt), weight(o_weight) {}
  node_id nid; 
  int cnt;
  double weight;
};

/**
 * @brief Definition of node.
 */
struct Node {
  Node(aggregation::Token &&o_token, /*int o_id,*/ int o_cnt, double o_weight) 
      : token(std::move(o_token)), 
        // id(o_id).
        cnt(o_cnt),
        weight(o_weight) {}
  Node(const aggregation::Token& o_token, const int o_cnt, const double o_weight)
      : token(o_token),
        cnt(o_cnt),
        weight(o_weight) {}
  //Node(std::pair<std::string, double> o) : name(std::move(o.first)), weight(o.second) {}
  // Token of this node 
  aggregation::Token token;
  // node id
  // node_id id;
  // Appearance times.
  int cnt;
  // Node weight.
  double weight;
  // The edges this node contains.
  std::vector<Edge> edges;
};



/**
 * @brief Definition of graph.
 */
class Graph {
 public:
  /**
   * @brief Add a new node into the graph. If the token is already in the
   *        graph, just find it's node_id and return.
   * @param token The token to be added to the graph. 
   * @param cnt How many times this token has appeared.
   * @param weight The weight of the token in the graph, can have different definition.
   *        Such as idf weight, frequency weight, hybrid weight.
   * @return The id of this token in the graph. It is also the index in the 
   *        nodes array.
   */
  node_id AddNode(const aggregation::Token& token, int cnt, double weight);

  /**
   * @brief Add a new node into the graph.
   * @param n The node to add.
   * @return The node id of this node. If it's a new node, add this node and 
   *         return the new id, otherwise return the id in the nodes_. 
   */
  node_id AddNode(const Node& n);


  /**
   * @brief Add a set of nodes into the graph.
   * @param first the begin of iterator of nodes.
   * @param last the end of iterator of nodes
   */
  void AddNodes(std::vector<Node>::iterator first,
                std::vector<Node>::iterator last);
  
  /**
   * @brief Add a new edge into the graph.
   * @param main_nid The node with main_nid should be of type entity.
   * @param nid No type limit on node of nid.
   * @param cnt How many times the nodes with main_nid and nide occur in the
   *        same post.
   * @param weight The weight of this edge in the graph. Can have multiple 
   *        definitions.
   */
  void AddEdge(node_id main_nid, node_id nid, int cnt, double weight);
  
  /**
   * @brief Get the node id of a token in the graph.
   * @param token The token.
   * @return The node_id of this token in the graph. -1 if there is no such token.
   */
  inline node_id GetNodeIdByToken(const aggregation::Token& token) {
    if (token2id_map_.find(token) != token2id_map_.end()) {
      return token2id_map_[token];
    } else {
      return -1;
    }
  }

  /**
   * @brief Get th token of a node given node id.
   * @param nid Node id.
   * @return the token of the node.
   */
   inline aggregation::Token& GetTokenByNodeId(const node_id nid) {
     return nodes_[nid].token;
   }

  /**
   * @brief Get the aggregation value of this graph. For example, the
   *        aggregation is on city and the graph is for "wisconsin madion", 
   *        then the function should return "wisconsin madison". 
   * @return The aggregation value.
   */
  inline std::string& agg_value() { return agg_value_; }
  
  /**
   * @brief Set the aggregation value for this graph.
   */
  void set_agg_value(std::string&& agg_value) {
    agg_value_ = std::move(agg_value);
  }

  void set_agg_value(const std::string& agg_value) {
    agg_value_ = agg_value;
  }
  /**
   * @brief Output this graph to file.
   * @param filepath the path of the output file.
   */
  void OutputToFile(const std::string& filepath);

  /**
   * @brief Output nodes information.
   */
  void OutputNodes() const;

  inline const std::vector<Node>& nodes() const { return nodes_; }

 private:
  // Nodes of this graph. Every node should have a unique token and node_id.
  std::vector<Node> nodes_;
  // The aggregation value of this grah. For example, if we aggregate on city,
  // this value should be the city name of this graph.
  std::string agg_value_;
  // The map from token to node_id. 
  std::unordered_map<aggregation::Token, node_id> token2id_map_;
};
} // namespace aggregation 

#endif // AGGREGATION_GRAPH_H_
