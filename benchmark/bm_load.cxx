#include "../cmd_parser/cmd_parser.h"
#include "../fm_index/fm_index.h"

#include <seqan3/search/fm_index/all.hpp>
#include <seqan3/search/search.hpp>
#include <seqan3/alphabet/aminoacid/aa27.hpp>

#include <fstream>
#include <chrono>

int main (int argc, char** argv) {

    std::vector<std::pair<char, std::pair<bool, std::string>>> options{
            {'u', {false, ""}},
            {'o', {false, ""}},
            {'s', {false, ""}},
            {'n', {false, ""}},
            {'c', {false, ""}},
    };

    CMD_Parser parser{options};

    parser.parse(argc, argv);

    std::string input_index_file_path{parser.get_argument('u')};
    std::vector<std::string> input_index_files{};

    std::string n{};
    size_t iterations = 1;

    if (parser.get_argument('n', n)) {
        iterations = std::stoi(n);
    }

    // Read list of index files
    if (parser.is_set('u')){
        std::cout << "[BM LOAD] Read input file\n";
        std::ifstream fin{input_index_file_path};

        // Check if file is open
        if (!fin.is_open()) {
            std::cerr << "[BM LOAD] Can't read input file!\n";
            return -1;
        } else {
            bool is_empty = (fin.get(), fin.eof());

            // Check if file is empty
            if (is_empty) {
                std::cerr << "[BM LOAD] Input file is empty!\n";
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

    std::string input_compressed_index_file_path{parser.get_argument('c')};
    std::vector<std::string> input_compressed_index_files{};

    // Read list of compressed index files
    if (parser.is_set('c')) {
        std::cout << "[BM LOAD] Read input file\n";
        std::ifstream fin{input_compressed_index_file_path};

        // Check if file is open
        if (!fin.is_open()) {
            std::cerr << "[BM LOAD] Can't read input file!\n";
            return -1;
        } else {
            bool is_empty = (fin.get(), fin.eof());

            // Check if file is empty
            if (is_empty) {
                std::cerr << "[BM LOAD] Input file is empty!\n";
                return -1;
            }
            fin.unget();
        }

        std::string line{};

        // Extract query file paths from file
        while (getline(fin, line)) {

            input_compressed_index_files.push_back(line);
        }

        fin.close();
    }

    std::string input_seqan_index_file_path{parser.get_argument('s')};
    std::vector<std::string> input_seqan_index_files{};

    // Read list of seqan index files
    if (parser.is_set('s')) {
        std::cout << "[BM LOAD] Read input file\n";
        std::ifstream fin{input_seqan_index_file_path};

        // Check if file is open
        if (!fin.is_open()) {
            std::cerr << "[BM LOAD] Can't read input file!\n";
            return -1;
        } else {
            bool is_empty = (fin.get(), fin.eof());

            // Check if file is empty
            if (is_empty) {
                std::cerr << "[BM LOAD] Input file is empty!\n";
                return -1;
            }
            fin.unget();
        }

        std::string line{};

        // Extract query file paths from file
        while (getline(fin, line)) {

            input_seqan_index_files.push_back(line);
        }

        fin.close();
    }

    std::vector<std::pair<double, std::string>> benchmarks{};
    benchmarks.reserve(iterations * input_index_files.size() +
                       input_compressed_index_files.size() * iterations);

    for (size_t i = 0; i < iterations; ++i ) {
        for (auto &index_file : input_index_files) {

            auto start = std::chrono::steady_clock::now();

            // Load Index
            FM_Index fm_index{};

            if (!fm_index.load_index(index_file)) {
                std::cerr << "[BM COMPRESSION] Can't load FM-Index\n";
                return 1;
            }

            auto end = std::chrono::steady_clock::now();
            std::chrono::duration<double> elapsed_seconds = end - start;

            std::cout << "[BM COMPRESSION] FM-Index loaded in " << elapsed_seconds.count() << " seconds.\n";

            benchmarks.emplace_back(std::make_pair(elapsed_seconds.count(), index_file));

        }

        for (auto &index_file : input_compressed_index_files) {

            auto start = std::chrono::steady_clock::now();

            // Load Index
            FM_Index fm_index{};

            if (!fm_index.load_index(index_file, true)) {
                std::cerr << "[BM COMPRESSION] Can't load FM-Index\n";
                return 1;
            }

            auto end = std::chrono::steady_clock::now();
            std::chrono::duration<double> elapsed_seconds = end - start;

            std::cout << "[BM COMPRESSION] FM-Index loaded in " << elapsed_seconds.count() << " seconds.\n";

            benchmarks.emplace_back(std::make_pair(elapsed_seconds.count(), index_file));
        }

        for (auto &index_file : input_seqan_index_files) {

            auto start = std::chrono::steady_clock::now();

            // Load Index
            seqan3::fm_index<seqan3::aa27,seqan3::text_layout::collection> seqan_fm_index{};
            std::ifstream fin{index_file, std::ios::binary};

            // Check if file is open
            if (!fin.is_open()) {
                std::cerr << "[INPUT] Can't read input file!\n";
            } else {
                bool is_empty = (fin.get(), fin.eof());

                // Check if file is empty
                if (is_empty) {
                    std::cerr << "[INPUT] Input file is empty\n";
                }
                fin.unget();
            }

            cereal::BinaryInputArchive iarchive{fin};
            iarchive(seqan_fm_index);
            fin.close();

            auto end = std::chrono::steady_clock::now();
            std::chrono::duration<double> elapsed_seconds = end - start;

           std::vector<seqan3::aa27_vector> seqan_queries{};

            auto search_results = seqan3::search(seqan_queries, seqan_fm_index);
            for (auto hit : search_results);

            std::cout << "[BM COMPRESSION] FM-Index loaded in " << elapsed_seconds.count() << " seconds.\n";

            benchmarks.emplace_back(std::make_pair(elapsed_seconds.count(), index_file));

        }
    }
    std::cout << "[BENCHMARK BUILD] Write benchmarks to file\n";

    std::string output_file{};

    if (parser.get_argument('o', output_file)) {

        std::ofstream fout(output_file, std::ios_base::app);

        if (fout.is_open()) {

            for (const std::pair<double, std::string> &benchmark : benchmarks) {
                fout << benchmark.first << "   " << benchmark.second << "\n";
            }

            fout.close();
        } else {
            std::cerr << "[BENCHMARK BUILD] Can't write output benchmarks file!\n";
            return -1;
        }
    } else {
        std::cerr << "[BM LOAD] Can't write output benchmark file!\n";
    }

    return 0;
}