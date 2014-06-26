// Copyright (c) 2014, Jing Fan. All Rights Reserved.

#ifndef AGGREGATION_IDF_GEN_H_
#define AGGREGATION_IDF_GEN_H_

#include <string>
#include <unordered_map>

#include "aggregation/util/option.h"

namespace aggregation {


/**
 * @brief generate idf for every word in a set of corpus
 */
class IdfGen {
 public:
  /**
   * @brief generate idf file
   * @param option Configure.
   * @param idf_map hashmap contains idf information, will be written in the function
   */
  void GenIdf(util::Option& option,
              std::unordered_map<std::string, double>& idf_map);
};


} // namespace aggregation

#endif // AGGREGATION_IDF_GEN_H_
