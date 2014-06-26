// Copyright (c) 2014, Jing Fan. All Rights Reserved.

#ifndef AGGREGATION_UTIL_SIMILARITY_H_
#define AGGREGATION_UTIL_SIMILARITY_H_

#include "aggregation/util/option.h"

namespace util {

/**
 * @brief Measure the similarity between two tag cloud.
 */
class Similarity {
 public:
  /**
   * @brief Constructor
   */
  Similarity();

  /**
   * @brief Get the similarity between two tag cloud files.
   * @param file1 The path of the first file.
   * @param file2 The path of the second file.
   * @return The similarity be in [0, 1]. 0 means that the two tag cloud are
   *         totally different and 1 means completely same.
   */
  double GetSimilarity(const std::string& file1, const std::string& file2);

 private:
  Option option_;
};

} // namespace util

#endif // AGGREGATION_UTIL_SIMILARITY_H_
