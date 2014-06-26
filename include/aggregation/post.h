// Copyright (c) 2014, Jing Fan. All Rights Reserved.

#ifndef AGGREGATION_POST_H_
#define AGGREGATION_POST_H_

#include <iostream>
#include <string>
#include <vector>

#include "aggregation/token.h"

namespace aggregation {

/**
 * @brief definition of basic twitter post
 */

struct Post {
  Post(std::vector<std::string>&& o_meta_info,
       std::vector<Token>&& o_tokens)
      : meta_info(std::move(o_meta_info)),
        tokens(std::move(o_tokens)) {}
 
  void Print() const {
    for (const auto& str : meta_info) {
      std::cout << str << " ";
    }
    for (const auto& t : tokens) {
      std::cout << t.name << " ";
    }
    std::cout << std::endl;
  }

  std::vector<std::string> meta_info;
  std::vector<Token> tokens;
};

} // namespace aggregation

#endif // AGGREGATION_POST_H_
