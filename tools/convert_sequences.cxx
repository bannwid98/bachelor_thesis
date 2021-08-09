#include <fstream>
#include <iostream>
#include <string>
#include <vector>

#include <regex>
#include <boost/algorithm/string.hpp>

#include "../cmd_parser/cmd_parser.h"

int main(int argc, char** argv) {

    // Parse commandline arguments
    std::vector<std::pair<char, std::pair<bool, std::string>>> options{
            {'i', {false, ""}}, {'o', {false, ""}}
    };

    CMD_Parser parser{options};

    parser.parse(argc, argv);

    std::string input_file{parser.get_argument('i')};
    std::string output_file{parser.get_argument('o')};

    // Read input file
    std::cout << "[CONVERT SEQUENCES] Read Input file\n";
    std::ifstream fin(input_file);

    // Check if file is open
    if (!fin.is_open()) {
        std::cerr << "[CONVERT SEQUENCES] Can't read input file!\n";
        return -1;
    }

    bool is_empty = (fin.get(), fin.eof());

    // Check if file is empty
    if (is_empty) {
        std::cerr << "[CONVERT SEQUENCES] Input file is empty\n";
    }

    fin.unget();

    std::string line{};
    std::vector<std::string> sequences{};
    std::vector<std::string> parts{};

    // Extract sequences from file
    while (getline(fin, line)) {
        boost::split(parts, line, boost::is_any_of("\t"));

        // Check if line contains a peptide
        if (parts[0] == "PEPTIDE") {
            sequences.push_back(parts[5]);
            continue;
        }

        // Check if line contains a protein
        if (parts[0] == "PROTEIN") {
            sequences.push_back(parts[6]);
            continue;
        }
    }

    fin.close();

    // Change sequence to unmodified sequence
    std::cout << "[CONVERT SEQUENCES] Convert to unmodified sequences\n";
    const std::regex base_regex("[A-Z]");

    std::vector<std::string> output{};

    for (size_t i = 0; i < sequences.size(); ++i) {
        std::string unmodified_sequence{};
        bool in_brackets = false;

        for (auto it = sequences[i].begin(); it < sequences[i].end(); ++it) {
            std::string temp{};
            temp.push_back(*it);

            if(*it == '(') in_brackets = true;

            // Check if character is not in brackets and a capital letter
            if(!in_brackets && std::regex_match(temp, base_regex)) unmodified_sequence.push_back(*it);

            if(*it == ')') in_brackets = false;
        }
        sequences[i]=unmodified_sequence;
    }

    std::ofstream fout(output_file);

    if (fout.is_open()) {
        for (const std::string & sequence : sequences) {

            if (sequence != "")
                fout << sequence << "\n";
        }

        fout.close();
    } else {
        std::cerr << "[CONVERT SEQUENCES] Can't write output file!\n";
    }
}