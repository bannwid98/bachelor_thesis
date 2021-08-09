#ifndef BACHELORARBEIT_CMD_PARSER_H
#define BACHELORARBEIT_CMD_PARSER_H

#include <string>
#include <vector>
#include <unordered_map>
#include <iostream>

class CMD_Parser {
private:
    std::unordered_map<char, std::string> arguments{};
    std::unordered_map<char, std::pair<bool, std::string>> options{};

public:
    // RULE OF SIX
    CMD_Parser() = default;
    CMD_Parser(CMD_Parser const &) = default;
    CMD_Parser(CMD_Parser &&) = default;
    CMD_Parser & operator=(CMD_Parser const &) = default;
    CMD_Parser & operator=(CMD_Parser &&) = default;
    ~CMD_Parser() = default;

    explicit CMD_Parser (const std::vector<std::pair<char, std::pair<bool, std::string>>> &input_options);

    bool get_argument (char flag, std::string &argument);
    std::string get_argument (char flag);
    bool parse(int argc, char** argv);
    bool is_set(char flage);


    std::unordered_map<char, std::pair<bool, std::string>> get_options();
    std::unordered_map<char, std::string> get_arguments();
};

#endif //BACHELORARBEIT_CMD_PARSER_H
