// Copyright (c) 2014, Jing Fan. All Rights Reserved.

#include "aggregation/ranker/ranker.h"

namespace aggregation {
bool CompareByCnt(const Node& a, const Node& b) {
  return a.cnt > b.cnt;
}


bool CompareByWeight(const Node& a, const Node& b) {
  return a.weight > b.weight;
}
} // namespace aggregation
