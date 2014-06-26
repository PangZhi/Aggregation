// Copyright (c) 2014, Jing Fan. All Rights Reserved.

#include "aggregation/aggregation.h"

#include <cmath> // log
#include <unordered_set>
#include <unordered_map>
#include <iostream>
#include <algorithm>
#include <fstream>
#include <cstring>

#include "aggregation/meta_data.h"
#include "aggregation/ranker/tfidf_ranker.h"
#include "aggregation/ranker/jaccard_ranker.h"
#include "aggregation/ranker/submodular_ranker.h"
#include "aggregation/post_reader.h"
#include "aggregation/arxiv_reader.h"

namespace aggregation {

Aggregation::Aggregation(const util::Option& option)
    : option_(option), meta_data_(MetaData(option_.GetOption("Schema"), option_.GetOption("Hierarchy"))) {
  // Initialize post_reader_ and read posts from files.
  if ("Twitter" == option_.GetOption("Type")) {
    reader_ = new PostReader(option_.GetOption("DataDirectory"));
  } else if ("Arxiv" == option_.GetOption("Type")){
    reader_ = new ArxivReader(option_.GetOption("DataDirectory"));
  } else {
    std::cout << "ERROR: Unknown type\n";
    exit(-1);
  }
  reader_->Read(posts_, meta_data_);
  // Initialize ranker_.
  if (option_.GetOption("RankMethod") == "Tfidf") {
    ranker_ = new TfidfRanker(option_.GetOption("CorpusDirectory")); 
  } else if (option_.GetOption("RankMethod") == "Jaccard") {
    ranker_ = new JaccardRanker();
  } else if (option_.GetOption("RankMethod") == "Submodular"){
    ranker_ = new SubmodularRanker(option_.GetOption("CorpusDirectory"));
  } else {
    std::cout << "ERROR: Unknown rank method\n";
    exit(-1);
  }
}

Aggregation::~Aggregation() {
  delete reader_;
  reader_ = nullptr;
  delete ranker_;
  ranker_ = nullptr;
}

void Aggregation::Aggregate() {
  ret_.clear();
  query_type output_query;
  if (meta_data_.GetNearestValidSuccessor(prev_queries_,
                                          cur_query_,
                                          atoi(option_.GetOption("ConfThrehold").c_str()),
                                          output_query)) {
    AggregateByGraph(output_query);
  } else {
    AggregateByPost();
  }
}
  // TODO(fanj):Add LRU update.
  // If there is limit on the maximum number of results that can be stored, need
  // to abandon some results.

void Aggregation::OutputResult() {
  GraphForest* forest = prev_rets_.back().get();
  for (auto& g : forest->graphs) {
    std::cout << "================== Graph for " << g->agg_value() 
              << " =====================\n";
    for (auto& n : g->nodes()) {
      if (n.token.token_type == TokenType::kEntity) {
        std::cout << n.token.name << " " << n.weight << " ";
        for (auto& e : n.edges) {
          Token& t = g->GetTokenByNodeId(e.nid);
          std::cout << t.name << " | ";
        }
        std::cout << "\n";
      }
    }
  }
} 

void Aggregation::AggregateByPost() {
  std::cout << "Function : AggregateByPost" << "\n";
  std::unordered_map<std::string, std::vector<Post> > group_by;

  for (Post& p : posts_) {
    group_by[p.meta_info[cur_query_.first]].push_back(p);
  }
  
  std::vector<std::unique_ptr<Graph> > graph_vec;
  
  for (auto x : group_by) {
    // filter category with too few posts. Mostly it's because of the wrong in
    // int twitter post
    if ("Yes" == option_.GetOption("PrintLimit")
        && atoi(option_.GetOption("PrintIndex").c_str()) == cur_query_.first
        && x.first != option_.GetOption("PrintValue")) {
      continue;
    }
    /* for (auto item : x.second) {
      for (Token& t : item.tokens) {
        std::cout << t.name << " ";
      }
      std::cout << "\n";
    }
    */
    // if (x.second.size() <= 100) {
    //  continue;
    //}
    std::unique_ptr<Graph> graph(new Graph);
    std::unique_ptr<Graph> backup_graph(new Graph);
    // Set aggregation value for graphs.
    graph->set_agg_value(x.first);
    backup_graph->set_agg_value(x.first);

    std::cout << "=======================================GenerateGraph for " 
              << x.first << "====================================" << "\n";
    // GenerateGraphByCnt(x.second, g);
    // Generate graph for every group by corpus.
    // GenerateGraphByTfidf(x.second, g.get());
    //Graph* a = graph.get();
    //Graph* b = backup_graph.get();
    ranker_->Rank(x.second,
                  cur_query_.second,
                  cur_query_.second,
                  graph.get(),
                  backup_graph.get());
    graph_vec.push_back(std::move(graph));
    ret_.push_back(std::move(backup_graph));
  }
  
  prev_queries_.push_back(cur_query_);
  prev_rets_.emplace_back(std::unique_ptr<GraphForest>(
        new GraphForest(std::move(graph_vec), cur_query_)));
} 

void Aggregation::AggregateByGraph(const query_type& child_query) {
  std::cout << "Function : AggregateByGraph" << "\n";
 
  int index = std::distance(prev_queries_.begin(),
                           std::find(prev_queries_.begin(),
                                     prev_queries_.end(),
                                     child_query));

  if (child_query.first == cur_query_.first) {
    // Same attribute.
    return;
  }
  // Get the result from low-level aggregation and group by graphs by
  // new aggregation dimension.
  std::unique_ptr<GraphForest>& forest = prev_rets_[index];
  std::unordered_map<std::string, std::vector<Graph*> > graph_map;
  
  for (auto& g : forest->graphs) { 
    graph_map[meta_data_.GetHighLevelMap(child_query.first, g->agg_value())].
      push_back(g.get());
  } 
  
  std::vector<std::unique_ptr<Graph> > graph_vec;
  
  for (auto pair : graph_map) {
    Graph* graph = new Graph;
    Graph* backup_graph = new Graph; 
    graph->set_agg_value(pair.first);
    backup_graph->set_agg_value(pair.first);
    std::cout << "=======================================GenerateGraph for "
              << pair.first << "===================================\n";
    // GenerateGraphByRollUp(pair.second, graph);
    ranker_->Rank(pair.second,
                  cur_query_.second,
                  cur_query_.second,
                  graph,
                  backup_graph);
    graph_vec.push_back(std::unique_ptr<Graph>(graph));
    ret_.push_back(std::unique_ptr<Graph>(backup_graph));
  }
  
  prev_rets_.emplace_back(std::unique_ptr<GraphForest>(
        new GraphForest(std::move(graph_vec), cur_query_)));
} 


void Aggregation::GenerateGraphByRollUp(std::vector<Graph*>& graphs, Graph* graph) {
  std::cout << "Function : GenerateGraphByRollUp" << "\n";
  std::unordered_map<Token, int> token_cnt_map;
  std::unordered_map<Token, std::unordered_set<Token> > co_occur_map;
  typedef std::pair<Token, Token> token_pair;
  std::unordered_map<token_pair, int> co_occur_cnt_map;

  for (auto g : graphs) {
    for (auto& node : g->nodes()) {
      if (node.token.token_type != TokenType::kEntity) {
        continue;
      }
      token_cnt_map[node.token] += node.cnt;
      for (auto& edge : node.edges) {
        Token& t = g->GetTokenByNodeId(edge.nid);
        co_occur_map[node.token].insert(t);
        co_occur_cnt_map[token_pair(node.token, t)] += edge.cnt;
      }
    }
  }

  ConvertMapToGraph(token_cnt_map, co_occur_map, co_occur_cnt_map, graph);
}

void Aggregation::ConvertMapToGraph(std::unordered_map<Token, int>& token_cnt_map,
                       std::unordered_map<Token, std::unordered_set<Token> >& co_occur_map,
                       std::unordered_map<token_pair, int>& co_occur_cnt_map,
                       Graph* graph) {
  for (auto item : token_cnt_map) {
    graph->AddNode(item.first, item.second, 
                   static_cast<double>(item.second) * (sqrt(idf_map_[item.first.name]) + 1));
  }
  for (auto item : co_occur_map) {
    node_id nid = graph->GetNodeIdByToken(item.first);
    for (auto item2 : item.second) {
      node_id nid2 = graph->AddNode(item2, 1, static_cast<double>(1));
      graph->AddEdge(nid, nid2, 1, static_cast<double>(1));
    }
  }
  std::vector<std::pair<Token, int> > token_vec;
}
} // namespace aggregation
