// Copyright (c) 2014, Jing Fan. All Rights Reserved.

#ifndef AGGREGATION_JACCARD_RANKER_RANKER_H_
#define AGGREGATION_JACCARD_RANKER_RANKER_H_

#include "aggregation/ranker/ranker.h"

namespace aggregation {
  
/**
 * @brief Subclass of ranker. Aggregate post and graph by using Jaccard
 *        similarity as scoring function.
 */
class JaccardRanker : public Ranker {
 public:
  void Rank(const std::vector<Post>& posts,
            const int knum,
            const int backup_num,
            Graph* g,
            Graph* backup_g);

  void Rank(const std::vector<Graph*>& graphs,
            const int knum,
            const int backup_num,
            Graph* g,
            Graph* backup_g);
 
 private:
  double GetJaccardSimlarity(const Post& p1, const Post& p2);
};

} // namespace aggregation 

#endif // AGGREGATION_JACCARD_RANKER_RANKER_H_
