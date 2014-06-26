// Copyright (c) 2014, Jing Fan. All Rights Reserved.

#ifndef AGGREGATION_RANKER_TFIDF_RANKER_H_
#define AGGREGATION_RANKER_TFIDF_RANKER_H_

#include <string>
#include <unordered_map>
#include <vector>

#include "aggregation/ranker/ranker.h"

namespace aggregation {

/**
 * @brief Ranker using tfidf as scoring function.
 */
class TfidfRanker : public Ranker {
 public:
  /**
   * @brief Constructor. The constructor should generate idf background
   *        information. If there is idf.txt under current directory, directly
   *        read from the file. Otherwise, generate from the corpus directory.
   */
  TfidfRanker(const std::string& corpus_path );

  void Rank(const std::vector<Post>& post,
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
  // Corpus path.
  const std::string corpus_path_;
  // Idf information.
  std::unordered_map<std::string, double> idf_map_;
};

} // namespace aggregation

#endif // AGGREGATION_RANKER_TFIDF_RANKER_H_
