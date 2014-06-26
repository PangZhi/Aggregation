// Copyright (c) 2014, Jing Fan. All Rights Reserved.

#ifndef AGGREGATION_ARXIV_READER_H_
#define AGGREGATION_ARXIV_READER_H_

#include <unordered_set>

#include "aggregation/reader.h"

namespace aggregation {

class MetaData;

/**
 * @brief read twitter post from file
 */
class ArxivReader : public Reader {
 public:
  /**
   * @brief constructor
   * @param data_path the path of directory of twitter data
   */
  ArxivReader(std::string&& data_path) : data_path_(std::move(data_path)) {}
  ArxivReader(const std::string& data_path) : data_path_(data_path) {}

  /**
   * @brief read twitter data and convert to posts
   * @param posts vector of posts to be overwritten
   * @param metadata Metadata to overwritten.
   */
  void Read(std::vector<Post>& posts, MetaData& metadata);
 private:
  // the path of directory where there is twitter data
  std::string data_path_;
};

}

#endif //AGGREGATION_POST_READER_H_ 
