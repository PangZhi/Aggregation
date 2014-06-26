// Copyright (c) 2014, Jing Fan. All Rights Reserved.

#ifndef AGGREGATION_RANKER_RANKER_H_
#define AGGREGATION_RANKER_RANKER_H_

#include <vector>

#include "aggregation/graph.h"
#include "aggregation/post.h"
#include "aggregation/util/hash_pair.h"

namespace aggregation {


bool CompareByCnt(const Node& a, const Node& b);

bool CompareByWeight(const Node& a, const Node& b);
/**
 * @brief Superclass of ranker. Aggregation will keep a pointer to ranker or
 *        its subclass and call the Rank function to get the highest ranked
 *        tokens.
 */
class Ranker {
 public: 
  /**
   * @brief Rank input and generate graph.
   * 
   * @param post Posts.
   * @param knum Keyword number.
   * @param backup_num Backup number.
   */
  virtual void Rank(const std::vector<Post>& post,
                   const int knum,
                   const int backup_num,
                   Graph* g,
                   Graph* backup_g) = 0;

  /**
   * @brief Rank nodes in input graphs and generate the graph.
   *
   * @param graphs Collections of low-level graphs.
   * @param knum Keyword number.
   * @param backup_num Backup number.
   */
  virtual void Rank(const std::vector<Graph*>& graphs,
                   const int knum,
                   const int backup_num,
                   Graph* g,
                   Graph* backup_g) = 0;
};

} // namespace aggregation

#endif // AGGREGATION_RANKER_RANKER_H_
