// Copyright (c) 2014, Jing Fan. All Rights Reserved.

#include <iostream>

#include "aggregation/util/similarity.h"

int main() {
  util::Similarity sim;
  std::cout << "Similarity is : " << sim.GetSimilarity("data/1.txt", "data/2.txt")
            << std::endl;
  return 1;
}
