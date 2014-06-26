#ifndef AGGREGATION_HASH_PAIR_H_
#define AGGREGATION_HASH_PAIR_H_

#include <cstddef>
#include <functional>
#include <utility>

namespace std {

  template <typename FirstT, typename SecondT>
  struct hash<pair<FirstT, SecondT>> {
    size_t operator()(const pair<FirstT, SecondT> &arg) const {
      size_t first_hash = hash<FirstT>()(arg.first);
      size_t second_hash = hash<SecondT>()(arg.second);
      // Based on Hash128to64() from cityhash
      const size_t kMul = 0x9ddfea08eb382d69ULL;
      size_t a = (first_hash ^ second_hash) * kMul;
      a ^= (a >> 47);
      size_t b = (second_hash ^ a) * kMul;
      b ^= (b >> 47);
      b *= kMul;
      return b;
    }
  };
} // namespace std
#endif  // AGGREGATION_HASH_PAIR_H_

