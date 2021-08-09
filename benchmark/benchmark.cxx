#include "../cmd_parser/cmd_parser.h"
#include "../fm_index/fm_index.h"

#include <seqan3/search/fm_index/all.hpp>
#include <seqan3/search/search.hpp>
#include <seqan3/alphabet/aminoacid/aa27.hpp>

#include "benchmark/benchmark.h"

FM_Index my_fm_index{};
std::vector<std::string> my_queries{};
std::vector<std::string> my_fasta{};
std::vector<struct hit> hits{};

seqan3::fm_index<seqan3::aa27,seqan3::text_layout::collection> seqan_fm_index{};

std::vector<seqan3::aa27_vector> seqan_queries{};
std::vector<seqan3::aa27_vector> seqan_fasta{};

/*void search_peptide_indexer() {
    std::system("/home/dominik/Development/OpenMS-build/bin/PeptideIndexer -in /home/dominik/Data/small.idXML -fasta /home/dominik/Data/10000seq.fasta -out /home/dominik/Data/hits.idXML -aaa_max 3 -decoy_string Y  >>/dev/null 2>>/dev/null");
}*/

void search_my_fm_index() {
     my_fm_index.backward_search(my_queries, hits, false);
}

void search_seqan_fm_index() {
    auto search_results = seqan3::search(seqan_queries, seqan_fm_index);
    for (auto hit : search_results);
}

void search_my_fm_index_with_aaa() {
    my_fm_index.backward_search(my_queries, hits, false,3);
}


static void BM_search_my_fm_index(benchmark::State& state) {

    for (auto _ : state) {

        state.PauseTiming();

        hits.clear();

        state.ResumeTiming();

        search_my_fm_index();
    }
}

static void BM_search_seqan_fm_index(benchmark::State& state) {

    for (auto _ : state)
        search_seqan_fm_index();
}

static void BM_search_my_fm_index_with_aaa(benchmark::State& state) {

    for (auto _ : state) {

        state.PauseTiming();

        hits.clear();

        state.ResumeTiming();

        search_my_fm_index_with_aaa();
    }
}

/*static void BM_search_peptide_indexer(benchmark::State& state) {

    for (auto _ : state)
        search_peptide_indexer();
}*/

BENCHMARK(BM_search_my_fm_index);
//BENCHMARK(BM_search_seqan_fm_index);
BENCHMARK(BM_search_my_fm_index_with_aaa);
//BENCHMARK(BM_search_peptide_indexer);
//BENCHMARK(BM_search_my_fm_index);
//BENCHMARK(BM_search_seqan_fm_index);
//BENCHMARK(BM_search_my_fm_index_with_aaa);
//BENCHMARK(BM_search_peptide_indexer);

int main(int argc, char** argv) {

    std::vector<std::pair<char, std::pair<bool, std::string>>> options{
            {'i', {false, ""}}, {'s', {false, ""}}, {'q', {false, ""}}, {'o', {false, ""}}
    };

    CMD_Parser parser{options};

    parser.parse(argc, argv);

    std::string index_file{parser.get_argument('i')};
    std::string query_file{parser.get_argument('q')};
    std::string seqan_index_file{parser.get_argument('s')};

    {
        std::cout << "[BENCHMARK] Read queries\n";
        std::ifstream fin{query_file};

        // Check if file is open
        if (!fin.is_open()) {
            std::cerr << "[BENCHMARK] Can't read input query file!\n";
            return 1;
        }

        bool is_empty = (fin.get(), fin.eof());

        // Check if file is empty
        if (is_empty) {
            std::cerr << "[BENCHMARK] Input query file is empty!\n";
            return 1;
        }

        fin.unget();

        std::string line{};

        while (getline(fin, line)) {
            my_queries.push_back(line);
        }

        fin.close();
    }

    for (const std::string &my_query : my_queries) {
        seqan3::aa27_vector seqan_query{};

        for (const char aa : my_query)
            seqan_query.push_back(seqan3::assign_char_to(aa, seqan3::aa27{}));

        seqan_queries.push_back(seqan_query);
    }

    // Read Index.file
    if (parser.is_set('s')) {
        std::ifstream fin{seqan_index_file, std::ios::binary};

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
    }

    std::cout << "[BENCHMARK] Load FM-Index\n";

    if (!my_fm_index.load_index(index_file)) {
        std::cerr << "[BENCHMARK] Can't load FM-Index\n";
        return 1;
    }

    int argc_BM = 4;
    char* argv_BM[argc_BM];

    std::string out{"--benchmark_out="};

    out += parser.get_argument('o');


    char* benchmark_out{(char*)out.c_str()};
    char* benchmark_out_format{(char*)"--benchmark_out_format=csv"};
    char* benchmark_repetitions{(char*)"--benchmark_repetitions=10"};

    argv_BM[1] = benchmark_out;
    argv_BM[2] = benchmark_out_format;
    argv_BM[3] = benchmark_repetitions;

    ::benchmark::Initialize(&argc_BM, argv_BM);
    if (::benchmark::ReportUnrecognizedArguments(argc_BM, argv_BM)) return 1;
    ::benchmark::RunSpecifiedBenchmarks();
}