// Copyright (c) 2014, Jing Fan. All Rights Reserved.

#ifndef AGGREGATION_META_DATA_H_
#define AGGREGATION_META_DATA_H_

#include <utility>
#include <string>
#include <vector>
#include <cctype>
#include <functional> // hash
#include <cstring> // size_t
#include <unordered_map>
#include <unordered_set>

#include "aggregation/util/option.h"

namespace aggregation {
/**
 * @brief Definition of query_type. Query should be <attribute, word number>.
 */
typedef std::pair<int, int> query_type;

/**
 * @brief Manage metadata.
 */
class MetaData {
 public:
  /**
   * @brief Constructor. 
   * @param schema_str The string indicates the shcema.
   * @param hierarchy_str The string indicates the hierarchy.
   */
  MetaData(const std::string& schema_str = "", const std::string& hierarchy_str = "");
  
  /**
   * @brief When read one post from file, add its meta data.
   * @param data The meta data of the post.
   */
  void AddMetaData(const std::vector<std::string>& data);

  /**
   * @brief Get the low-level to high-level map. 
   * @param ll Low-level index.
   * @param s Low-level string.
   * @return The high-level string.
   */
  inline std::string& GetHighLevelMap(const int ll, const std::string& s) {
    return high_level_maps[ll][s];
  }
  
  /**
   * @brief Get nearest and valid one from previous query. Nearest means the
   *        dimension should have the shortest path from the dimension of
   *        current query. Valid means the keyword number is enough to 
   *        generate a safe result(the confidence is greater than the 
   *        confidence threhold).
   *
   * @param prev_queries Previouse queries.
   * @param cur_query Current query.
   * @param conf_threhold Confidence threhold.
   * @param output_query The query to be overwritten.
   * @return True if there is such query, false otherwise.
   */
  bool GetNearestValidSuccessor(std::vector<query_type>& prev_queries,
                           const query_type& cur_query,
                           const int conf_threhold,
                           query_type& output_query);

  /**
   * @brief Get scale ratio between dimensions, which is determined by the
   *        number of subgraphs when go from successor_dim to ascent_dim.
   * 
   * @param ascent_dim Ascent dimension.
   * @param successor_dim Successor dimension.
   * @return Scale ratio.
   */
  double GetScaleRatio(const int ascent_dim,
                       const int successor_dim);

  /**
   * @brief Get length of path from ascent_dim to successor_dim.
   *
   * @param ascent_dim Ascent dimension.
   * @param successor_dim Successor dimension.
   * @return Length of path.
   */
  int GetPathLen(const int ascent_dim,
                 const int successor_dim);

  /**
   * @brief Get the parent dimension.
   *
   * @param dim The dimension.
   * @return The parent dimension.
   */
  inline int GetParentDim(const int dim) {
    if (parent_dim_map_.find(dim) != parent_dim_map_.end()) {
      return parent_dim_map_[dim];
    } else {
      return dim;
    }
  }

 private:
  /**
   * @brief Get confidence for get aggregation result for a by directly using
   * result of aggregation for b.
   * @param a Current query.
   * @param b One of previous query.
   */
  double GetConf(const query_type& a, const query_type& b);

  // Attribute name.
  std::vector<std::string> schema_;
  // Used to record the possbile options for every attribute.
  std::vector<std::unordered_set<std::string> > data_;
  // The map from low-level to high-level. 
  std::unordered_map<int, std::unordered_map<std::string, std::string> > high_level_maps;
 
  // The direct low-level dimension for every dimension. If the child dimension
  // of one is itself, then this dimension has no child dimension.
  std::unordered_map<int, int> child_dim_map_;

  // The direct high level dimension for every dimension. If the parent
  // dimension of one is itself, then this dimension has no parent dimension.
  std::unordered_map<int, int> parent_dim_map_;
};

} // namespace aggregation


#endif // AGGREGATION_META_DATA_H_
