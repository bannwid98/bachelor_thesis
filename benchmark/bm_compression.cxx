#include "../cmd_parser/cmd_parser.h"
#include "../fm_index/fm_index.h"

#include <chrono>
#include <tuple>

int main (int argc, char** argv) {

    std::vector<std::pair<char, std::pair<bool, std::string>>> options{
            {'i', {false, ""}},
            {'o', {false, ""}},
            {'n', {false, ""}},
            {'q', {false, ""}},
            {'c', {false, ""}},
            {'v', {false, ""}}
    };

    CMD_Parser parser{options};

    parser.parse(argc, argv);

    std::string input_file_path{parser.get_argument('q')};
    std::vector<std::string> input_files{};

    // Read list of query files
    {
        std::cout << "[BM COMPRESSION] Read input file\n";
        std::ifstream fin{input_file_path};

        // Check if file is open
        if (!fin.is_open()) {
            std::cerr << "[BM COMPRESSION] Can't read input file!\n";
            return -1;
        } else {
            bool is_empty = (fin.get(), fin.eof());

            // Check if file is empty
            if (is_empty) {
                std::cerr << "[BM COMPRESSION] Input file is empty!\n";
                return -1;
            }
            fin.unget();
        }

        std::string line{};

        // Extract query file paths from file
        while (getline(fin, line)) {

            input_files.push_back(line);
        }

        fin.close();
    }

    std::string input_index_file_path{parser.get_argument('i')};
    std::vector<std::string> input_index_files{};

    // Read list of query files
    {
        std::cout << "[BM COMPRESSION] Read input file\n";
        std::ifstream fin{input_index_file_path};

        // Check if file is open
        if (!fin.is_open()) {
            std::cerr << "[BM COMPRESSION] Can't read input file!\n";
            return -1;
        } else {
            bool is_empty = (fin.get(), fin.eof());

            // Check if file is empty
            if (is_empty) {
                std::cerr << "[BM COMPRESSION] Input file is empty!\n";
                return -1;
            }
            fin.unget();
        }

        std::string line{};

        // Extract query file paths from file
        while (getline(fin, line)) {

            input_index_files.push_back(line);
        }

        fin.close();
    }

    std::vector<std::tuple<double, std::string, std::string>> benchmarks{};
    benchmarks.reserve(input_files.size() * input_index_files.size());

    std::string n{};
    size_t iterations = 1;

    if (parser.get_argument('n', n)) {
        iterations = std::stoi(n);
    }

    for (size_t k = 0; k < input_index_files.size(); ++k) {

        const std::string &index_file = input_index_files[k];

        std::cout << "[BM COMPRESSION] Benchmark index nr. " << k + 1 << " of " << input_files.size() <<
                            " " << index_file << "\n";

        // Search all queries
        for (size_t i = 0; i < input_files.size(); ++i) {

            const std::string &input_query = input_files[i];

            std::cout << "[BM COMPRESSION] Read in query file " << i + 1 << " of " << input_files.size() << "\n";

            std::vector<std::string> queries{};
            std::ifstream fin{input_query};

            // Check if file is open
            if (!fin.is_open()) {
                std::cerr << "[BM COMPRESSION] Can't read query file nr. " << i << "!\n";
                return -1;
            } else {
                bool is_empty = (fin.get(), fin.eof());

                // Check if file is empty
                if (is_empty) {
                    std::cerr << "[BM COMPRESSION] Query file nr. " << i << " is empty!\n";
                }
                fin.unget();
            }

            std::string line{};

            // Extract sequences from file
            while (getline(fin, line)) {

                queries.push_back(line);
            }

            fin.close();

            for (size_t j = 0; j < iterations; ++j) {

                std::cout << "[BM COMPRESSION] Iteration " << j + 1 << " of " << iterations << "\n";

                auto start = std::chrono::steady_clock::now();

                // Load Index
                FM_Index fm_index{};

                if (!fm_index.load_index(index_file, parser.is_set('c'))) {
                    std::cerr << "[BM COMPRESSION] Can't load FM-Index\n";
                    return 1;
                }

                std::vector<struct hit> hits{};

                fm_index.backward_search(queries, hits);

                auto end = std::chrono::steady_clock::now();
                std::chrono::duration<double> elapsed_seconds = end - start;

                if (hits.size() == 1) {
                    std::cout << "[BM COMPRESSION] FM-Index loaded and " << hits.size() << " query is found in " <<
                              elapsed_seconds.count() << " seconds.\n";
                } else {
                    std::cout << "[BM COMPRESSION] FM-Index loaded and " << hits.size() << " queries are found in " <<
                              elapsed_seconds.count() << " seconds.\n";
                }

                benchmarks.emplace_back(std::make_tuple(elapsed_seconds.count(), input_query, index_file));
            }

            std::cout << "[BENCHMARK BUILD] Write benchmarks to file\n";

            std::string output_file{};

            if (parser.get_argument('o', output_file)) {

                std::ofstream fout(output_file, std::ios_base::app);

                if (fout.is_open()) {

                    for (const std::tuple<double, std::string, std::string> &benchmark : benchmarks) {
                        fout << std::get<0>(benchmark) << "   " << std::get<1>(benchmark) << "   " << std::get<2>(benchmark) << "\n";
                    }

                    fout.close();
                } else {
                    std::cerr << "[BENCHMARK BUILD] Can't write output benchmarks file!\n";
                    return -1;
                }
            }
        }
    }

    return 0;
}
