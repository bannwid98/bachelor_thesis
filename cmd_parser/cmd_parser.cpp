#include "cmd_parser.h"

bool CMD_Parser::parse(int argc, char **argv) {

    for (int i = 1; i < argc;) {

        std::string flag{argv[i]};
        ++i;

        if (flag[0] != '-') {
            std::cerr << "[CMD PARSER] No commandline flag!\n";
            return false;
        }

        if (flag.size() > 2) {
            std::cerr << "[CMD PARSER] Flag is more then one character\n";
            return false;
        }

        // CHECK: if flag is a positional option
        auto search = this->options.find(flag[1]);
        if (search == this->options.end()) {
        //if (!flags.contains(flag[1])) {
                std::cerr << "[CMD PARSER] Unknown flag!\n";
                return false;
        }

        // CHECK: if flag is positional
        if (search->second.first) {
            arguments.insert({flag[1], ""});
        } else {

            // IF NOT: CHECK: if argument transmitted
            if (i < argc) {
                std::string argument = argv[i];
                ++i;

                // CHECK: if argument is not a flag
                if (argument[0] == '-') {
                    std::cerr << "[CMD PARSER] Missing argument for option" << flag << "\n";
                    return false;
                }

                arguments.insert({flag[1], argument});
            }
        }

        // Print message
        if (!search->second.second.empty()) {
            std::cout << search->second.second << "\n";
        }

    } // for (int i = 1; i < argc;)

    return true;
}

CMD_Parser::CMD_Parser(const std::vector<std::pair<char, std::pair<bool, std::string>>> &input_options) {

    for (const std::pair<char, std::pair<bool, std::string>> &input_option : input_options) {
        this->options.insert(input_option);
    }
}

bool CMD_Parser::get_argument(const char flag, std::string &argument) {

    auto search = this->arguments.find(flag);

    if (search == this->arguments.end()) {
        return false;
    } else {
        argument = search->second;
        return true;
    }
}

std::string CMD_Parser::get_argument(char flag) {

    auto search = this->arguments.find(flag);

    if (search == this->arguments.end()) {
        return "";
    } else {
        return search->second;
    }
}

std::unordered_map<char, std::pair<bool, std::string>> CMD_Parser::get_options() {
    return this->options;
}

std::unordered_map<char, std::string> CMD_Parser::get_arguments() {
    return this->arguments;
}

bool CMD_Parser::is_set(const char flag) {
    auto search = this->arguments.find(flag);

    if (search == this->arguments.end()) {
        return false;
    } else {
        return true;
    }
}
