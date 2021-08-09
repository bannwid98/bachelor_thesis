#include "../cmd_parser/cmd_parser.h"

#include <cstddef> // size_t
#include <fstream>
#include <iostream>
#include <vector>
#include <string>

void generate_sequences(size_t nr_of_sequences, size_t minimum_sequence_size,
                        std::vector<std::string> & sequences, size_t variance = 0);

int main (int argc, char** argv) {

    std::vector<std::pair<char, std::pair<bool, std::string>>> options{
            {'f', {false, ""}}, {'q', {false, ""}},
            {'s', {false, ""}}, {'l', {false, ""}}, {'v', {false, ""}}
    };

    CMD_Parser parser{options};

    parser.parse(argc, argv);

    std::string output_file{};

    std::cout << "[GENERATE SEQUENCES] Generate data\n";
    std::vector<std::string> sequences{};

    std::string size{};
    std::string length{};

    if (!(parser.get_argument('l', length) && parser.get_argument('s', size))) {

        std::cerr << "[GENERATE SEQUENCES] Missing minimum length of sequences or size of sequence file!\n";
        return 1;
    }

    std::string varaince{};

    if (parser.get_argument('v', varaince)) {
        generate_sequences(std::stoi(size), std::stoi(length), sequences,std::stoi(varaince));
    } else {
        generate_sequences(std::stoi(size), std::stoi(length), sequences);
    }


    if (parser.get_argument('f', output_file)) {

        std::ofstream fout(output_file);

        if (fout.is_open()) {

            std::cout << "[GENERATE SEQUENCES] Write FastA file \n";
            size_t counter = 0;

            for (const std::string &sequence : sequences) {

                fout << ">seq" << counter << "\n";
                fout << sequence << "\n";
                ++counter;
            }

            fout.close();
        } else {
            std::cerr << "[GENERATE SEQUENCES] Can't write output FastA file!\n";
        }
    }

    if (parser.get_argument('q', output_file)) {

        std::ofstream fout(output_file);

        if (fout.is_open()) {

            std::cout << "[GENERATE SEQUENCES] Write query file \n";

            for (const std::string &sequence : sequences) {

                fout << sequence << "\n";
            }

            fout.close();
        } else {
            std::cerr << "[GENERATE SEQUENCES] Can't write output query file!\n";
        }
    }
}

void generate_sequences(const size_t nr_of_sequences, const size_t minimum_sequence_size,
                        std::vector<std::string> & sequences, const size_t variance) {

    for (int j = 0; j < nr_of_sequences; ++j) {

        size_t length{};

        if (variance != 0) length = (random() % variance) + minimum_sequence_size;
        else length = minimum_sequence_size;

        std::string sequence{};
        sequence.reserve(length);

        for (size_t i = 0; i < length; ++i)
            sequence.push_back(static_cast<char>((random() % 26) + 65));

        sequences.push_back(sequence);
    }
}