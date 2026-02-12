//
// Created by Shixuan Sun on 11/5/20.
//

#ifndef XTRAGRAPHCOMPUTING_COMMAND_PARSER_H
#define XTRAGRAPHCOMPUTING_COMMAND_PARSER_H

#include <string>
#include <algorithm>
#include <numeric>
#include <vector>

class InputParser{
public:
    InputParser (int &argc, char **argv){
        for (int i = 1; i < argc; ++i)
            tokens_.emplace_back(argv[i]);
    }

    std::string get_cmd_option(const std::string &option) const{
        auto itr = std::find_if(tokens_.begin(), tokens_.end(),
                                [&option](const std::string& token) {
                                    return token == option || normalize_option_token(token) == option;
                                });
        if (itr != tokens_.end() && ++itr != tokens_.end()){
            return *itr;
        }
        return "";
    }

    bool check_cmd_option_exists(const std::string &option) const{
        return std::find_if(tokens_.begin(), tokens_.end(),
                            [&option](const std::string& token) {
                                return token == option || normalize_option_token(token) == option;
                            }) != tokens_.end();
    }

    std::string get_cmd() {
        return std::accumulate(tokens_.begin(), tokens_.end(), std::string(" "));
    }

private:
    static std::string normalize_option_token(const std::string& token) {
        if (token.size() > 2 && token[0] == '-' && token[1] == '-') {
            auto first_non_hyphen = token.find_first_not_of('-');
            if (first_non_hyphen != std::string::npos) {
                return "-" + token.substr(first_non_hyphen);
            }
        }
        return token;
    }

    std::vector<std::string> tokens_;
};

#endif //XTRAGRAPHCOMPUTING_COMMAND_PARSER_H
