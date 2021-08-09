#include "../fm_index/fm_index.h"
#include "../cmd_parser/cmd_parser.h"

#include <iostream>
#include <chrono>
#include <vector>
#include <string>

int main(int argc, char** argv) {

    std::vector<std::pair<char, std::pair<bool, std::string>>> options{
            {'f', {false, ""}}, {'i', {false, ""}}, {'c', {true, ""}}
    };

    CMD_Parser parser{options};

    parser.parse(argc, argv);

    std::string input_file{parser.get_argument('f')};
    std::string output_file{ parser.get_argument('i')};

    std::vector<std::string> fasta{};

    std::cout << "[BUILD INDEX] Read FastA file\n";
    std::ifstream fin{input_file};

    // Check if file is open
    if (!fin.is_open()) {
        std::cerr << "[BUILD INDEX] Can't read input file!\n";
    }   else {
        bool is_empty = (fin.get(), fin.eof());

        // Check if file is empty
        if (is_empty) {
            std::cerr << "[BUILD INDEX] Input file is empty!\n";
        }
        fin.unget();
    }

    std::string line{};
    std::string sequence{};

    // Extract sequences from file
    while (getline(fin, line)) {

        if (*line.begin() != '>') {
            sequence.append(line);
        } else {
            fasta.push_back(sequence);
            sequence.clear();
        }
    }

    fin.close();

    std::cout << "[BUILD INDEX] Build index\n";

    auto start = std::chrono::steady_clock::now();

    FM_Index fm_index{fasta};

    auto end = std::chrono::steady_clock::now();
    std::chrono::duration<double> elapsed_seconds = end - start;

    std::cout << "[BUILD INDEX] Index built up in " << elapsed_seconds.count() << " seconds.\n";

    std::cout << "[BUILD INDEX] Write index\n";

    if (fm_index.write_index(output_file, parser.is_set('c'))){
        std::cout << "[BUILD INDEX] Index successfully written to disk\n";
        return 0;
    } else {
        std::cerr << "[BUIlD INDEX] Can't write index to disk!\n";
        return 1;
    }
}
