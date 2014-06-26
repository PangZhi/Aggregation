// Copyright (c) 2014, Jing Fan. All Rights Reserved.

#ifndef AGGREGATION_AGGREGATION_H_
#define AGGREGATION_AGGREGATION_H_

#include <climits>
#include <memory>
#include <unordered_map>

#include "aggregation/graph.h"
#include "aggregation/post.h"
#include "aggregation/reader.h"
#include "aggregation/util/hash_pair.h"
#include "aggregation/idf_gen.h"
#include "aggregation/util/option.h"
#include "aggregation/meta_data.h"

namespace aggregation {

class MetaData;
class Ranker;

/**
 * @brief Definition of query(aggregation dimension and keyword number).
 */
typedef std::pair<int, int> query_type;

/**
 * @brief structure of the result of aggregation
 */
struct GraphForest {
  GraphForest(std::vector<std::unique_ptr<Graph> >&& o_graphs,
              const query_type& o_query) :
      graphs(std::move(o_graphs)), query(o_query) {}
  
  // Aggregation result.
  std::vector<std::unique_ptr<Graph> > graphs;
  // Query.
  query_type query; 
  GraphForest(GraphForest&& gf) = default;
};


/**
 * @brief Definition of token pair.
 */
typedef std::pair<Token, Token> token_pair;
  

/**
 * @brief aggregation class
 */
class Aggregation {
 public:
  /**
   * @brief Constructor
   * @option The program settings.
   */
  Aggregation(const util::Option& option);
  
  /**
   * @brief Move constructor.
   */
  Aggregation(Aggregation&& a) = default;
  
  /**
   * @brief Destructor.
   */
  ~Aggregation();

  /**
   * @brief Set aggregation dimension and keyword number.
   * @param agg_dim Aggregation Dimension.
   * @param knum Keyword number.
   */
  inline void set_query(const int agg_dim, const int knum = INT_MAX) {
    cur_query_ = query_type(agg_dim, knum);
  }

  /**
   * @brief Aggregate, the result will be pushed into the prev_result_._
   */
  void Aggregate();

  /**
   * @brief Output graph to files.
   */
  void OutputResult();

 private:
  /**
   * @brief Aggregate by iterating over all the post.
   */
  void AggregateByPost();

  /**
   * @brief Aggregate by merging low level graphs.
   * @param child_query The query that has useful lower level information.
   */
  void AggregateByGraph(const query_type& child_query);

  /**
   * @brief Generate graph from low-level graphs.
   * @param graphs Low-level graphs.
   * @param graph The place to store the result.
   */
  void GenerateGraphByRollUp(std::vector<Graph*>& graphs, Graph* graph);

  /**
   * @brief Convert the hashmaps to graph. 
   * @param token_cnt_map std::unordered_map<Token, int>, The hashmap from tokens to how
   *        many times they have appeared.
   * @param co_occur_map std::unordered_map<Token, std::unordered_set<Token> >,
   *        the hashmap from token(Entity) to other tokens(Event) which have
   *        co-occured with the entity in at least one twitter post.
   * @param co_occur_cnt_map How many times different token pairs have occurred
   *        in the same twitter post.
   * @param graph The graph to be overwritten.
   */
  void ConvertMapToGraph(std::unordered_map<Token, int>& token_cnt_map,
                         std::unordered_map<Token, std::unordered_set<Token> >& co_occur_map,
                         std::unordered_map<token_pair, int>& co_occur_cnt_map,
                         Graph* graph);

  
  // Program setting.
  util::Option option_;
  // MetaData.
  MetaData meta_data_;
  // pointer to the reader
  Reader* reader_;
  // Pointer to the ranker.
  // std::unique_ptr<Ranker> ranker_;
  Ranker* ranker_;
  // Previouse aggregation. The length can be limited to a constant number so that do not
  // occupy a lot of memory. Use LRU to update the queue.
  std::vector<query_type> prev_queries_;
  // The result of previous query.
  std::vector<std::unique_ptr<GraphForest> > prev_rets_;
  // The result of this query. 
  std::vector<std::unique_ptr<Graph> > ret_;
  // Posts after nlp process.
  std::vector<Post> posts_;
  // Current query, include aggregation dimension and keyword number.
  query_type cur_query_;
  // record idf information in corpus
  std::unordered_map<std::string, double> idf_map_;
};

} // namespace aggregation
#endif // AGGREGATION_AGGREGATION_H_
