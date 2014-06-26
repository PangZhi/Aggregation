// Copyright (c) 2014, Jing Fan. All Rights Reserved.

#ifndef AGGREGATION_READER_H_
#define AGGREGATION_READER_H_

#include <vector>

#include "aggregation/post.h"

namespace aggregation {

class MetaData;

/**
 * @brief Base class for all readers.
 */
class Reader {
 public:
  virtual void Read(std::vector<Post>& posts, MetaData& metadata) = 0;
};

} // namespace aggregation

#endif
