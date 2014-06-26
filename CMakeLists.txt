cmake_minimum_required(VERSION 2.8)

SET(CMAKE_CXX_FLAGS_RELEASE "$ENV{CXXFLAGS} -O3 -Wall")

SET(CMAKE_CXX_FLAGS_DEBUG "$ENV{CXXFLAGS}  -g")

SET(CMAKE_BUILD_TYPE Debug)

project(Aggregation CXX C)

# Look for flags to enable C++11 support, if any.
include(CheckCXXCompilerFlag)
CHECK_CXX_COMPILER_FLAG("-std=c++11" COMPILER_HAS_STD_CXX11)
if (COMPILER_HAS_STD_CXX11)
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11")
else()
    CHECK_CXX_COMPILER_FLAG("-std=c++0x" COMPILER_HAS_STD_CXX0X)
    if (COMPILER_HAS_STD_CXX0X)
        set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++0x")
    endif()
endif()

include_directories(./include/)


set(EXECUTABLE_OUTPUT_PATH bin)

add_executable(
  main
  main.cc
  src/post_reader.cc
  src/arxiv_reader.cc
  src/aggregation.cc
  src/idf_gen.cc
  src/graph.cc
  src/option.cc
  src/meta_data.cc
  src/jaccard_ranker.cc 
  src/tfidf_ranker.cc
  src/submodular_ranker.cc
  src/ranker.cc
  src/similarity.cc
  src/string_util.cc
  )



add_executable(
  util_idf
  util/util_idf.cc
  src/idf_gen.cc 
  src/option.cc
  src/post_reader.cc
  src/arxiv_reader.cc
  src/graph.cc
  src/meta_data.cc
  src/string_util.cc
  )

add_executable(
  util_similarity
  util/util_similarity.cc 
  src/similarity.cc
  src/option.cc
  src/string_util.cc
  )

add_executable(
  util_preprocess
  util/util_preprocess.cc 
  src/preprocess.cc
  src/string_util.cc
  )
