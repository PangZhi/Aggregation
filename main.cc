// Copyright (c) 2014, Jing Fan. All Rights Reserved.

#include <aggregation/post_reader.h>

#include <cstdlib>
#include <iostream>

#include <aggregation/util/hash_pair.h>
#include <aggregation/util/option.h>
#include <hrtimer.h>
/*
int main() { 
aggregation::PostReader reader("/scratch/TwitterTag2/");
  std::vector<aggregation::Post> posts;
  reader.Read(posts);
  for (aggregation::Post p : posts) {
    std::cout << p.date << " | " << p.hour << " | " << p.state << " | " << p.city << " | ";
    for (aggregation::Token t : p.tokens) {
      std::cout << t.name << " : " << (int)t.token_type << " | ";
    }
    std::cout << std::endl;
  }
  return 1;
}

*/

#include "aggregation/aggregation.h"

static hrtimer_t timer;

int main(int argc, char** argv) {
  std::cout << "usage: ./bin/main and enter aggregation_dimension"
            << " in (0, 1, 2, 3)\n"
            << "0: Date\n"
            << "1: Hour\n"
            << "2: State\n"
            << "3: City\n" << std::endl;
  util::Option option;
  option.Parse();
  aggregation::Aggregation agg(option);
  //int dim = atoi(argv[1]);
  int dim;
  int knum;
  initTimer(&timer);
  while(1) {
    std::cout << "Please enter the aggregation dimension and keyword number\n";
    std::cin >> dim >> knum;
    agg.set_query(dim, knum);
    std::cout << "aggregation dimension is : " << dim  
              << ", knum is " << knum << "\n";
    startTimer(&timer);
    agg.Aggregate();
    stopTimer(&timer);
    std::cout << "Time is : " << (double)(getTimerNs(&timer)) / 1e9
              << " s" << std::endl;
  }
  
}
