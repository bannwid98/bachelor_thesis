#include "../cmd_parser/cmd_parser.h"

#include <chrono>
#include <iostream>
#include <fstream>

int main (int argc, char** argv) {

    std::vector<std::pair<char, std::pair<bool, std::string>>> options{
            {'i', {false, ""}},
            {'o', {false, ""}},
            {'n', {false, ""}}
    };

    CMD_Parser parser{options};

    parser.parse(argc, argv);

    std::string input_file_path{parser.get_argument('i')};

    std::vector<std::string> commands{};

    // Read list of commands
    {
        std::cout << "[BM TOOL] Read input file\n";
        std::ifstream fin{input_file_path};

        // Check if file is open
        if (!fin.is_open()) {
            std::cerr << "[BM TOOL] Can't read input file!\n";
            return -1;
        } else {
            bool is_empty = (fin.get(), fin.eof());

            // Check if file is empty
            if (is_empty) {
                std::cerr << "[BM TOOL] Input file is empty!\n";
                return -1;
            }
            fin.unget();
        }

        std::string line{};

        // Extract sequences from file
        while (getline(fin, line)) {

            commands.push_back(line);
        }

        fin.close();
    }

    std::string n{};
    size_t iterations = 1;

    if (parser.get_argument('n',n)) {
        iterations = std::stoi(n);
    }

    std::vector<std::pair<double, std::string>> benchmarks{};
    benchmarks.reserve(commands.size()*iterations);

    // Run commands
    for (size_t i = 0; i < commands.size(); ++i) {

        std::string command = commands[i];

        command += ">>/dev/null 2>>/dev/null";

        std::cout << "[BM TOOL] Run command " << i + 1 << " of " << commands.size() << "\n";

        for (size_t j = 0; j < iterations; ++j) {

            std::cout << "[BM TOOL] Iteration " << j + 1 << " of " << iterations << "\n";

            auto start = std::chrono::steady_clock::now();

            std::system(command.c_str());

            auto end = std::chrono::steady_clock::now();
            std::chrono::duration<double> elapsed_seconds = end - start;

            std::cout << "[BM TOOL] Running command takes " << elapsed_seconds.count() << " seconds.\n";

            benchmarks.emplace_back(std::make_pair(elapsed_seconds.count(), command));
        } // for (size_t j = 0; j < iterations; ++j)
    } // for (size_t i = 0; i < commands.size(); ++i)

    std::cout << "[BM TOOL] Write benchmarks to file\n";

    std::string output_file{};

    if (parser.get_argument('o', output_file)) {

        std::ofstream fout(output_file, std::ios_base::app);

        if (fout.is_open()) {

            for (const std::pair<double, std::string> &benchmark : benchmarks) {
                fout << benchmark.first << "   " << benchmark.second << "\n";
            }

            fout.close();
        } else {
            std::cerr << "[BM TOOL] Can't write output benchmarks file!\n";
            return -1;
        }
    }

    return 0;
}
