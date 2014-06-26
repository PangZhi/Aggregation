// Copyright (c) 2014, Jing Fan. All Rights Reserved.

#include <aggregation/idf_gen.h>
#include "aggregation/util/option.h"

int main() {
  util::Option option;
  option.Parse();
  aggregation::IdfGen idf_gen;
  std::unordered_map<std::string, double> idf_map;
  idf_gen.GenIdf(option, idf_map);
  return 0;
}
