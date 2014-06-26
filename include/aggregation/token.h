// Copyright (c) 2014, Jing Fan. All Rights Reserved.

#ifndef AGGREGATION_TOKEN_H_
#define AGGREGATION_TOKEN_H_

#include <algorithm>
#include <string>
#include <utility>
#include <cctype>
#include <functional> // hash
#include <cstring> // size_t

namespace aggregation {

/**
 * @brief the definition of basic token in every twitter post
 */

/**
 * @brief token type
 */
enum class TokenType {
  // other
  kOther = 0,
  // entity
  kEntity,
  // event
  kEvent
};

//TODO(fanj):move lower and trim to other places.
struct Token {
  Token(std::string&& o_name, const TokenType &  o_token_type)
      : name(std::move(o_name)), token_type(o_token_type) {
    std::transform(name.begin(), name.end(), name.begin(), ::tolower);
    
    while (name.length() != 0 && ispunct(name.front())) {
      name.erase(0, 1);
    }
    while (name.length() != 0 && ispunct(name.back())) {
      name.pop_back();
    }
  }

  Token(const std::string& o_name, const TokenType &o_token_type)
      : name(o_name), token_type(o_token_type) {}

  bool operator == (const Token& o) const {
    return (name == o.name && token_type == o.token_type);  
  }

  // name of this token, can be phrase
  std::string name;
  // token type
  TokenType token_type;
};

} // namespace aggregation

namespace std {

template<>
struct hash<aggregation::Token> {
  std::size_t operator()(const aggregation::Token& t) const {
    using std::size_t;
    using std::hash;
    using std::string;
    return ((hash<string>()(t.name)
            ^ (hash<int>()(static_cast<int>(t.token_type)) << 1)) >> 1);
  }
};
} // namespace std

#endif // AGGREGATION_TOKEN_H_
