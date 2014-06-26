// Copyright (c) 2014, Jing Fan. All Rights Reserved.

#ifndef AGGREGATION_PREPROCESS_H_ 
#define AGGREGATION_PREPROCESS_H_

#include <string>
#include <unordered_set>
#include <vector>

#include "lib/english_stem.h"

namespace aggregation {
namespace util {
/**
 * @brief Preprocess class. Given a raw string, it will remove stop words and
 *        do stemming.
 */
class Preprocess{
 public:
  /**
   * @brief constructor
   */
  Preprocess();

  /**
   * @brief deconstructor
   */
  ~Preprocess() {}

  /**
   * @brief process a string
   * @param str the string to be processed
   * @param results vector to store the result words
   */
  void Process(const char* str, std::vector<std::string>& results);
  
 private:
  // Stop words.
  std::unordered_set<std::string> stopwords_set_;
  // Stem engine.
  stemming::english_stem<> stem_english_;
  
  /**
   * @brief judge whether the word is a valid word
   */
  bool IsValid(const std::string& s);
  
  inline void AddStopWord(const std::string& s) {
      stopwords_set_.insert(s);
  }
};

} // namespace util
} // namespace aggregation
#endif // AGGREGATION_PREPROCESS_H_
