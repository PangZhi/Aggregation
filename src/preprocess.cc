// Copyright (c) 2014, Jing Fan. All Rights Reserved.

#include "aggregation/util/preprocess.h"

#include <algorithm>
#include <fstream>
#include <iostream>
#include <iterator>
#include <locale>
#include <sstream>
#include <string>

namespace aggregation {
namespace util {

Preprocess::Preprocess(){
  std::ifstream fin("input/stopword.list");
  std::string s;
  while(!fin.eof()){
    fin >> s;
    stopwords_set_.insert(s);
  }
}

bool Preprocess::IsValid(const std::string& s){
  for (int i = 0 ; i < s.length(); i++){
    if ( !( (s[i]>='a' && s[i] <='z') || ( s[i] >= 'A' && s[i] <= 'Z' ) 
        || (s[i] >= '0' && s[i] <= '1')) ) {
      return false;
    }
  }
  if (stopwords_set_.find(s) != stopwords_set_.end()) {
    return false;
  }
  return true;
}

void Preprocess::Process(const char* str, std::vector<std::string>& results) {
  std::string document(str);
  if (document.length() == 0) {
    return;
  }
  std::transform(document.begin(), document.end(), document.begin(), ::tolower);

  std::stringstream ss(document);
  std::istream_iterator<std::string> begin(ss);
  std::istream_iterator<std::string> end;
    
  for (auto iter = begin; iter != end; ++iter) {
    /*
    wchar_t *wstr = new wchar_t[iter->length() + 1];
    mbstowcs(wstr, iter->c_str(), iter->length() + 1);
    wstring wstring(wstr);
    stem_english_(wstring);
    char *s= new char[wstring.length() + 1];
    wcstombs(s, wstring.c_str(), wstring.length() + 1);
    delete[] wstr;
    string new_s(s); 
    if (IsValid(new_s)) {
        results.push_back(new_s);
    }
    */
    if (IsValid(*iter)) {
        results.push_back(*iter);
    }
  }
}

} // namespace util
} // namespace aggregation
