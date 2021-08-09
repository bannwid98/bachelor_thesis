#include "../cmd_parser/cmd_parser.h"
#include "../fm_index/fm_index.h"

#include <chrono>

std::ifstream::pos_type filesize(const std::string &filename)
{
    std::ifstream in(filename, std::ifstream::ate | std::ifstream::binary);
    return in.tellg();
}

int main (int argc, char** argv) {

    std::vector<std::pair<char, std::pair<bool, std::string>>> options{
            {'f', {false, ""}}, {'o', {false, ""}}, {'n', {false, ""}}, {'v', {false, ""}}
    };

    CMD_Parser parser{options};

    parser.parse(argc, argv);

    std::string input_file_path{parser.get_argument('f')};
    std::vector<std::string> input_files{};

    // Read list of fasta files
    {
        std::cout << "[BENCHMARK BUILD] Read input file\n";
        std::ifstream fin{input_file_path};

        // Check if file is open
        if (!fin.is_open()) {
            std::cerr << "[BENCHMARK BUILD] Can't read input file!\n";
            return -1;
        } else {
            bool is_empty = (fin.get(), fin.eof());

            // Check if file is empty
            if (is_empty) {
                std::cerr << "[BENCHMARK BUILD] Input file is empty!\n";
                return -1;
            }
            fin.unget();
        }

        std::string line{};

        // Extract sequences from file
        while (getline(fin, line)) {

            input_files.push_back(line);
        }

        fin.close();
    }

    std::vector<std::pair<double, size_t>> benchmarks{};
    benchmarks.reserve(input_files.size());
    
    std::string n{};
    size_t iterations = 1;
    
    if (parser.get_argument('n',n)) {
        iterations = std::stoi(n);
    }

    // Build all indices
    for (size_t i = 0; i < input_files.size(); ++i) {

        const std::string &input_fasta = input_files[i];

        std::cout << "[BENCHMARK BUILD] Read in FastA file " << i+1 << " of " << input_files.size() << "\n";

        std::vector<std::string> fasta{};
        std::ifstream fin{input_fasta};

        // Check if file is open
        if (!fin.is_open()) {
            std::cerr << "[BENCHMARK BUILD] Can't read fasta nr. " << i << " file!\n";
            return -1;
        }   else {
            bool is_empty = (fin.get(), fin.eof());

            // Check if file is empty
            if (is_empty) {
                std::cerr << "[BENCHMARK BUILD] Fasta file nr. " << i << " is empty!\n";
            }
            fin.unget();
        }

        std::string line{};

        // Extract sequences from file
        while (getline(fin, line)) {

            if (line[0] != '>')
                fasta.push_back(line);
        }

        fin.close();

        std::cout << "[BENCHMARK BUILD] Build index of file " << input_fasta << "\n";

        size_t file_size = filesize(input_fasta);

        for (size_t j = 0; j < iterations; ++j) {

            std::cout << "[BENCHMARK BUILD] Iteration " << j+1 << " of " << iterations << "\n";

            auto start = std::chrono::steady_clock::now();

            FM_Index fm_index{fasta};

            auto end = std::chrono::steady_clock::now();
            std::chrono::duration<double> elapsed_seconds = end - start;

            std::cout << "[BENCHMARK BUILD] Index built up in " << elapsed_seconds.count() << " seconds.\n";

            benchmarks.emplace_back(std::make_pair(elapsed_seconds.count(), file_size));
        }
    }

    std::cout << "[BENCHMARK BUILD] Write benchmarks to file\n";

    std::string output_file{};

    std::string version = parser.get_argument('v');

    if (parser.get_argument('o', output_file)) {

        std::ofstream fout(output_file, std::ios_base::app);

        if (fout.is_open()) {

            for (const std::pair<double, size_t> &benchmark : benchmarks) {
                fout << benchmark.first << "   " << benchmark.second << " " << version << "\n";
            }

            fout.close();
        } else {
            std::cerr << "[BENCHMARK BUILD] Can't write output benchmarks file!\n";
            return -1;
        }
    }
    return 0;
}