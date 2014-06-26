// Copyright (c) 2014, Jing Fan. All Rights Reserved.

#include "aggregation/post_reader.h"

#include <dirent.h>
#include <algorithm>
#include <cstdio>
#include <cstring>
#include <fstream>
#include <iostream>
#include <unordered_map>
#include <unordered_set>

#include "aggregation/meta_data.h"

#define BUF_SIZE 1024


namespace aggregation {

std::unordered_map<std::string, std::string> month_map {
  {"Jan", "01"},
  {"Feb", "02"}
};

std::unordered_map<std::string, int> type_map{
  {"O", 0},
  {"BENTITY", 1},
  {"IENTITY", 1},
  {"BEVENT", 2},
  {"IEVENT", 2}
};

void PostReader::Read(std::vector<Post>& posts,
                      MetaData& metadata) {
  DIR* dirp = nullptr;
  struct dirent* direntp = nullptr;
  dirp = opendir(data_path_.c_str());
  if ( nullptr == dirp ) { std::cout  << "can't find dir \n"; }
  if (nullptr != dirp) {
    while ((direntp = readdir(dirp)) != nullptr) {
      if (strncmp(direntp->d_name, ".", 1) == 0) {
        continue;
      }
      std::cout << direntp->d_name << std::endl;      
      // std::ifstream fin(data_path_ + std::string(direntp->d_name));
      char buf[BUF_SIZE];
      memset(buf, 0, BUF_SIZE);
      strcat(buf, data_path_.c_str());
      strcat(buf, "/");
      strcat(buf, direntp->d_name);
      std::cout << buf << std::endl;
      FILE* fin = fopen(buf, "r");
      if (nullptr == fin) {
        std::cout << "ERROR: can't open file " << buf << "\n";
        continue;
      }
      char time[BUF_SIZE];
      char location[BUF_SIZE];
      char tokens[BUF_SIZE];
      char city[BUF_SIZE];
      char state[BUF_SIZE];
      char year[BUF_SIZE];
      char month[BUF_SIZE];
      char day[BUF_SIZE];
      char hour[BUF_SIZE];
      
      int cnt = 0;
      while (!feof(fin)) {
        fscanf(fin, "%[^\t]\t%[^\t]", location, time);
        if (strcmp(location, "NULL") == 0) {
          fgets(buf, BUF_SIZE, fin);
          continue;
        } 
        // std::cout << location << " | " << time << " | " << tokens << "\n";
        
        fgets(tokens, BUF_SIZE, fin);
        tokens[0]=' ';
        if (strlen(tokens) > 0 ) {
          tokens[strlen(tokens)-1] = '\0';
        }
        sscanf(location, "%[^,], %[^\t]", city, state);
        // std::cout << city << " " << state << "\n";
        sscanf(time, "%s %s %s %[^:]:%s %s %[^\t]\t", buf, month, day, hour, buf, buf, year);
        // std::cout << month_map[std::string(month)] << " " <<  day  << " "<< hour << " " << year << "\n";
        strcat(year, "/");
        strcat(year, month_map[std::string(month)].c_str());
        strcat(year, "/");
        strcat(year, day);
        
        const char* split = " ";
        char* p = nullptr;
        p = strtok(tokens, split);
        char token[BUF_SIZE];
        char entity[BUF_SIZE];
        char event[BUF_SIZE];
        char prev_token[BUF_SIZE];
        int prev_type = 0;
        std::vector<Token> tokens;

        while (p != nullptr) {
          sscanf(p, "%[^;];%[^;];%[^ \t]", token, entity, event);
          // std::cout << token << " | " << entity << " | " << event << "\n";
          int type = std::max(type_map[std::string(entity)], type_map[std::string(event)]); 
          // std::cout << token << " | " << entity << " | " << entity << " | " << type << "\n";
          if (0 == type) {
            if (0 != prev_type) {
              tokens.push_back(Token(std::string(prev_token), (aggregation::TokenType)prev_type));
              memset(prev_token, 0, BUF_SIZE);
              prev_type = 0;
            }
          } else {
            if (entity[0] == 'B' || event[0] == 'B') {
              if (prev_type != 0) {
                tokens.push_back(Token(std::string(prev_token), (aggregation::TokenType)prev_type));
              }
              strcpy(prev_token, token);
              // std::cout << prev_token << std::endl;
              prev_type = type;
            }
            else {
              // std::cout << prev_token << std::endl;
              strcat(prev_token, " ");
              strcat(prev_token, token);
              // std::cout << prev_token << std::endl;
            }
          }
          p = strtok(nullptr, split);
        }
        if (0 != prev_type) {
          tokens.push_back(Token(std::string(prev_token), (aggregation::TokenType)prev_type));
        }

        // Update hierarchy map.
        // city_state_map[std::move(std::move(std::string(state) + " " + std::string(city)))]
        //    = std::string(state);
        // hour_date_map[std::move(std::string(year) + " " + std::string(hour))]
        //    = std::string(year);
        std::vector<std::string> data_vec{std::string(year),
                             std::string(year) + " " + std::string(hour),
                             std::string(state),
                             std::string(state) + " " + std::string(city)};
        metadata.AddMetaData(data_vec);
    
        posts.push_back(Post(std::move(data_vec),
                            std::move(tokens)));
        cnt ++;
        if (cnt % 10000 == 0) {
          std::cout << "Read post " << cnt << "\n";
        } 
      }
      fclose(fin);
    }
  }
  closedir(dirp);

  /**
  for (aggregation::Post p : posts) {
    std::cout << p.date << " | " << p.hour << " | " << p.state << " | " << p.city << " | ";
    for (aggregation::Token t : p.tokens) {
      std::cout << t.name << " : " << (int)t.token_type << " | ";
    }
    std::cout << std::endl;
  }
  **/
}

} // namespace aggregation

