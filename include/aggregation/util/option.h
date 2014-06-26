// Copyright (c) 2014, Jing Fan. All Rights Reserved.

#ifndef AGGREGATION_UTIL_OPTION_H_
#define AGGREGATION_UTIL_OPTION_H_

#include <iostream>
#include <string>
#include <unordered_map>

namespace util {

/**
 * @brief Parse the command line or from config file.
 */
class Option {
 public:
  /**
   * @brief Get the option setting by pass the option string.
   * @param s The option string, should be on in kOptionStrs.
   * @return The option setting. KeywordNumber : 5 should return 5.
   */
  std::string& GetOption(const std::string& s) ;
  
  /**
   * @brief Parse config file and load the settings. The default config file is
   *        "Config.txt" under root directory of this program.
   */
  void Parse();

  /**
   * @brief Parse the command line.
   * @param argc The number of command line arguments.
   * @param argv The command line arguments.
   */
  void Parse(const int argc, const char** argv);

 private:
  // Possible setting strings, maybe not useful
  const std::string kOptionStrs[5] = {"KeywordNumber",
                                      "DataDirectory",
                                      "CorpusDirectory",
                                      "ConfThrehold",
                                      "RankMethod"};
  
  // The map from option string to option setting.
  std::unordered_map<std::string, std::string> option_map_ = {
    {"KeywordNumber", "20"},
    {"DataDirectory", "~/TwitterTag2/"},
    {"CorpusDirectory", "~/corpus/"},
    {"Confidence", "0.95"},
    {"RankMethod", "Tfidf"}
  };
};

} // namespace util


#endif // AGGREGATION_UTIL_OPTION_H_
