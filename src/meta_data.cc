// Copyright (c) 2014, Jing Fan. All Rights Reserved.

#include "aggregation/meta_data.h"

#include <assert.h>
#include <cstdlib>

#include "aggregation/util/string_util.h"

namespace aggregation {

MetaData::MetaData(const std::string& schema_str, const std::string& hierarchy_str) {
  this->schema_ = std::move(util::Split(schema_str, ';'));
  std::vector<std::string> hierarchy_pair = std::move(util::Split(hierarchy_str, ';'));
  for (const std::string& item : hierarchy_pair) {
    std::vector<std::string> indexes = std::move(util::Split(item, '-'));
    assert(indexes.size() == 2);
    int child = atoi(indexes[0].c_str());
    int parent = atoi(indexes[1].c_str());
    this->child_dim_map_[parent] = child;
    this->parent_dim_map_[child] = parent;
  }
  this->data_.resize(schema_.size());
  std::cout << "schema size : "  << schema_.size() << "\n";
}

void MetaData::AddMetaData(const std::vector<std::string>& data) {
  for (int i = 0; i < data.size(); ++i) {
    this->data_[i].insert(data[i]);
  }
  for (const auto& item : parent_dim_map_) {
    high_level_maps[item.first][data[item.first]] = data[item.second];
  }

}

double MetaData::GetConf(const query_type& a, const query_type& b) {
  return 1;
}

bool MetaData::GetNearestValidSuccessor(std::vector<query_type>& prev_queries_,
                                   const query_type& cur_query,
                                   const int conf_threhold,
                                   query_type& output_query) {
  std::unordered_set<int> prev_dims;
  std::unordered_map<int, std::unordered_set<int> > dim_num_map;
  for (auto q : prev_queries_) {
    prev_dims.insert(q.first);
    dim_num_map[q.first].insert(q.second);
  }

  int cur_dim = cur_query.first;
  while (1) {
    if (prev_dims.find(cur_dim) != prev_dims.end()) {
      for (auto knum : dim_num_map[cur_dim]) {
        if (GetConf(cur_query, query_type(cur_dim, knum)) > conf_threhold) {
          output_query = query_type(cur_dim, knum);
          return true;
        }
      }
    }
    if (child_dim_map_.find(cur_dim) == child_dim_map_.end()) {
      break;
    }
    cur_dim = child_dim_map_[cur_dim];
  }
  return false;
}


double GetScaleRatio(const int ascent_dim,
                     const int successor_dim) {
  // TODO(fanj):Finish this function.
  return 1;
}

int GetPathLen(const int ascent_dim,
    const int successor_dim) {
  // TODO(fanj):Finish this function.
  return 1;
}

} // namespace aggregation
