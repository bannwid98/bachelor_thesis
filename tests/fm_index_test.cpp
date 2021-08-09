#include "../fm_index/fm_index.h"
#include "../helper_functions.h"

#include <chrono>
#include <functional>
#include <algorithm>

void generate_sequences(const size_t fasta_size, const size_t minimum_sequence_size, std::vector<std::string> & sequences) {

    for (int j = 0; j < fasta_size; ++j) {

        size_t length = minimum_sequence_size;

        std::string sequence{};
        sequence.reserve(length);

        for (size_t i = 0; i < length; ++i)
            sequence.push_back(static_cast<char>((random() % 26) + 65));

        sequences.push_back(sequence);
    }
}

void small_test() {

    std::vector<std::string> mini_test_fasta{"KXABXA", "ABXBA"};
    std::vector<std::pair<uint32_t, uint16_t>> mini_test_sa{{0,4}, {1,5}, {0,1},
                                                            {1,1}, {1,4}, {1,0},
                                                            {0, 2}, {1,2}, {0, 3},
                                                            {0, 0}, {1,3}};

    FM_Index test_index{mini_test_fasta};

    std::vector<std::string> queries{"ABX", "XA", "BA"};

    {
        std::cout<< "Without check\n";

        std::vector<struct hit> hits{};

        auto start = std::chrono::steady_clock::now();
        test_index.backward_search(queries, hits, false);
        auto end = std::chrono::steady_clock::now();
        std::chrono::duration<double> elapsed_seconds = end - start;

        if (hits.size() == 1) {
            std::cout << "[TEST FM INDEX] " << hits.size() << " query is found in " <<
                      elapsed_seconds.count() << " seconds." << std::endl;
        } else {
            std::cout << "[TEST FM INDEX] " << hits.size() << " queries are found in " <<
                      elapsed_seconds.count() << " seconds." << std::endl;
        }

        std::cout << "[TEST FM INDEX] Check hits" << std::endl;

        if (check_hits(queries, hits, mini_test_fasta)) {
            std::cout << "[TEST FM INDEX] ALL HITS FOUND CORRECTLY\n";
        } else {
            std::cerr << "[TEST FM INDEX] SOME HITS ARE WRONG!\n";
        }
    }

    {
        std::cout<< "With check\n";

        std::vector<struct hit> hits{};

        auto start = std::chrono::steady_clock::now();
        test_index.backward_search(queries, hits, true);
        auto end = std::chrono::steady_clock::now();
        std::chrono::duration<double> elapsed_seconds = end - start;

        if (hits.size() == 1) {
            std::cout << "[TEST FM INDEX] " << hits.size() << " query is found in " <<
                      elapsed_seconds.count() << " seconds." << std::endl;
        } else {
            std::cout << "[TEST FM INDEX] " << hits.size() << " queries are found in " <<
                      elapsed_seconds.count() << " seconds." << std::endl;
        }

        std::cout << "[TEST FM INDEX] Check hits" << std::endl;

        if (check_hits(queries, hits, mini_test_fasta)) {
            std::cout << "[TEST FM INDEX] ALL HITS FOUND CORRECTLY\n";
        } else {
            std::cerr << "[TEST FM INDEX] SOME HITS ARE WRONG!\n";
        }
    }

    // WRITE INDEX
    {
        std::cout << "[TEST FM INDEX] Write index to disk" << std::endl;

        if (test_index.write_index("/home/dominik//CLionProjects/Bachelorarbeit/test_data/fm_index.file")) {
            std::cout << "[TEST FM INDEX] INDEX SUCCESSFULLY WRITTEN" << std::endl;
        } else {
            std::cerr << "[TEST FM INDEX] INDEX COULD NOT BE WRITTEN TO DISK!" << std::endl;
            return;
        }
    }

    FM_Index new_index{};

    // LOAD INDEX
    {
        std::cout << "[TEST FM INDEX] Load index from hard drive" << std::endl;

        if (new_index.load_index("/home/dominik//CLionProjects/Bachelorarbeit/test_data/fm_index.file")) {
            std::cout << "[TEST FM INDEX] INDEX COULD BE LOADED" << std::endl;
        } else {
            std::cerr << "[TEST FM INDEX] INDEX COULDN'T BE LOADED" << std::endl;
            return;
        }
    }

    // CHECK: if everything is correctly loaded
    /*{
        if (test_index.operator==(new_index)) {
            std::cout << "[TEST FM INDEX] THE LOADED INDEX IS EQUAL AS THE WRITTEN ONE" << std::endl;
        } else {
            std::cerr << "[TEST FM INDEX] THE LOADED INDEX IS UNEQUAL AS THE WRITTEN ONE!" << std::endl;
        }
    }*/
}

void test_build () {

    std::cout << "[TEST FM INDEX] BUILD FM INDEX FROM MULTIPLE SEQUENCES" << std::endl;
    std::cout << "[TEST FM INDEX] Generate Data" << std::endl;

    std::vector<std::string> fasta{};
    generate_sequences(1'000, 100, fasta);

    std::cout << "[TEST FM INDEX] Build Index" << std::endl;

    FM_Index test_index{fasta};

    // CHECK SUFFIX ARRAY
    {
        std::cout << "[TEST FM INDEX] Check suffix array" << std::endl;

        std::vector<std::pair<uint32_t, uint16_t>> sa{test_index.get_suffix_array()};

        if (is_sorted(fasta, sa)) {
            std::cout << "[TEST FM INDEX] SUFFIX ARRAY IS SORTED" << std::endl;
        } else {
            std::cerr << "[TEST FM INDEX] SUFFIX ARRAY IS NOT SORTED!" << std::endl;
        }
    }

    // CHECK BWT
    {
        std::cout << "[TEST FM INDEX] Check BWT" << std::endl;

        std::string bwt{test_index.get_bwt()};
        std::vector<std::pair<uint32_t, uint16_t>> sa{test_index.get_suffix_array()};

        if (check_bwt(fasta, bwt, sa)) {
            std::cout << "[TEST FM INDEX] BWT IS CORRECT" << std::endl;
        } else {
            std::cerr << "[TEST FM INDEX] BWT IS INCORRECT!" << std::endl;
        }
    }

    // CHECK OCCURRENCE MATRIX
    {
        std::cout << "[TEST FM INDEX] Control occurrence matrix" << std::endl;

        std::vector<std::array<uint32_t, 27>> occ_mat = test_index.get_occurrence_matrix();
        std::string bwt{test_index.get_bwt()};

        if (control_occ_mat(bwt, occ_mat)) {
            std::cout << "[TEST FM INDEX] OCCURRENCE MATRIX IS CORRECT COMPUTED" << std::endl;
        } else {
            std::cerr << "[TEST FM INDEX] OCCURRENCE MATRIX IS INCORRECT COMPUTED" << std::endl;
        }
    }

    // CHECK COUNT ARRAY
    {
        std::cout << "[TEST FM INDEX] Check count array" << std::endl;

        std::array<size_t, 27> count_arr{test_index.get_cumulated_count()};

        if (test_count_array(fasta, count_arr)) {
            std::cout << "[TEST FM INDEX] COUNT ARRAY IS CORRECT" << std::endl;
        } else {
            std::cerr << "[TEST FM INDEX] COUNT ARRAY IS INCORRECT" << std::endl;
        }
    }
}

void test_read_and_write() {

    std::cout << "[TEST FM INDEX] BUILD FM INDEX FROM MULTIPLE SEQUENCES" << std::endl;
    std::cout << "[TEST FM INDEX] Generate Data" << std::endl;

    std::vector<std::string> fasta{};
    generate_sequences(10'000, 1000, fasta);

    std::cout << "[TEST FM INDEX] Build Index" << std::endl;

    FM_Index test_index{fasta};

    // WRITE INDEX
    {
        std::cout << "[TEST FM INDEX] Write index to disk" << std::endl;

        if (test_index.write_index("/home/dominik/Data/fm_index.file", false)) {
            std::cout << "[TEST FM INDEX] INDEX SUCCESSFULLY WRITTEN" << std::endl;
        } else {
            std::cerr << "[TEST FM INDEX] INDEX COULD NOT BE WRITTEN TO DISK!" << std::endl;
            return;
        }
    }

    FM_Index new_index{};

    // LOAD INDEX
    {
        std::cout << "[TEST FM INDEX] Load index from hard drive" << std::endl;
        auto start_build = std::chrono::steady_clock::now();

        if (new_index.load_index("/home/dominik/Data/fm_index.file", false)) {
            auto end_build = std::chrono::steady_clock::now();
            std::chrono::duration<double> elapsed_seconds_build = end_build - start_build;
            std::cout << "[TEST FM INDEX] INDEX COULD BE LOADED in " << elapsed_seconds_build.count() << " seconds" << std::endl;
        } else {
            std::cerr << "[TEST FM INDEX] INDEX COULDN'T BE LOADED" << std::endl;
            return;
        }
    }

    // CHECK: if everything is correctly loaded
    {
        if (test_index.operator==(new_index)) {
            std::cout << "[TEST FM INDEX] THE LOADED INDEX IS EQUAL AS THE WRITTEN ONE" << std::endl;
        } else {
            std::cerr << "[TEST FM INDEX] THE LOADED INDEX IS UNEQUAL AS THE WRITTEN ONE!" << std::endl;
        }
    }
}

/*void test_find_all() {

    std::cout << "[TEST FM INDEX] TEST IF ALL OCCURRENCES ARE CORRECTLY LOCATED" << std::endl;
    std::cout << "[TEST FM INDEX] Generate Data" << std::endl;

    std::vector<std::string> fasta{};
    generate_sequences(1'000, 100, fasta);
    std::vector<std::string> queries{};
    generate_sequences(1'000, 5, queries);

    std::vector<hit> hits_boyer_moor{};
    std::vector<hit> hits_fm_index{};

    {
        std::cout << "[TEST FM INDEX] Search with boyer moor\n";

        auto start = std::chrono::steady_clock::now();

        for (size_t j = 0; j < queries.size(); ++j) {
            std::string &query = queries[j];

            struct hit finding{};
            finding.query = j;
            for (size_t i = 0; i < fasta.size(); ++i) {

                std::string &sequence = fasta[i];

                auto it = sequence.begin();

                while (it != sequence.end()) {
                    it = std::search(it+1, sequence.end(),
                                     std::boyer_moore_searcher(query.begin(), query.end()));

                    if (it != sequence.end()) {
                        finding.hit_vec.push_back(std::make_pair<uint32_t, uint16_t>(i, it - sequence.begin()));
                    }
                }
            }
            if (!finding.hit_vec.empty()) {
                hits_boyer_moor.push_back(finding);
            }
        }

        auto end = std::chrono::steady_clock::now();
        std::chrono::duration<double> elapsed_seconds = end - start;

        if (hits_boyer_moor.size() == 1) {
            std::cout << "[TEST FM INDEX] " << hits_boyer_moor.size() << " query is found in " <<
                      elapsed_seconds.count() << " seconds." << std::endl;
        } else {
            std::cout << "[TEST FM INDEX] " << hits_boyer_moor.size() << " queries are found in " <<
                      elapsed_seconds.count() << " seconds." << std::endl;
        }
    }

    {

        std::cout << "[TEST FM INDEX] Build Index" << std::endl;

        FM_Index test_index{fasta};

        std::cout << "[TEST FM INDEX] Search queries" << std::endl;

        auto start = std::chrono::steady_clock::now();
        test_index.backward_search(queries, hits_fm_index);
        auto end = std::chrono::steady_clock::now();
        std::chrono::duration<double> elapsed_seconds = end - start;

        if (hits_fm_index.size() == 1) {
            std::cout << "[TEST FM INDEX] " << hits_fm_index.size() << " query is found in " <<
                      elapsed_seconds.count() << " seconds." << std::endl;
        } else {
            std::cout << "[TEST FM INDEX] " << hits_fm_index.size() << " queries are found in " <<
                      elapsed_seconds.count() << " seconds." << std::endl;
        }
    }

    if (hits_boyer_moor.size() == hits_fm_index.size()) {
        for (size_t i = 0; i < hits_fm_index.size(); ++i) {
            const hit &hit_boyer_moor = hits_boyer_moor[i];
            const hit &hit_fm_index = hits_fm_index[i];

            if (hit_fm_index.query != hit_boyer_moor.query) {
                std::cout << "[TEST FM INDEX] Hits are unequal " << hit_fm_index.query << " " << hit_boyer_moor.query;
            }
        }
    }
}*/

void test_search_queries() {

    std::cout << "[TEST FM INDEX] BUILD FM INDEX FROM MULTIPLE SEQUENCES" << std::endl;
    std::cout << "[TEST FM INDEX] Generate Data" << std::endl;

    std::vector<std::string> fasta{};
    generate_sequences(1'000, 100, fasta);
    std::vector<std::string> queries{};
    generate_sequences(100'000, 6, queries);
    queries.emplace_back("HICYIE");
    std::cout << "[TEST FM INDEX] Build Index in ";

    auto start_build = std::chrono::steady_clock::now();
    FM_Index test_index{fasta};
    auto end_build = std::chrono::steady_clock::now();
    std::chrono::duration<double> elapsed_seconds_build = end_build - start_build;

    std::cout << elapsed_seconds_build.count() << " seconds." << std::endl;

    {
        std::cout << "[TEST FM INDEX] Search queries without check " << std::endl;

        std::vector<hit> hits{};
        auto start = std::chrono::steady_clock::now();
        test_index.backward_search(queries, hits, false);
        auto end = std::chrono::steady_clock::now();
        std::chrono::duration<double> elapsed_seconds = end - start;

        if (hits.size() == 1) {
            std::cout << "[TEST FM INDEX] " << hits.size() << " query is found in " <<
                      elapsed_seconds.count() << " seconds." << std::endl;
        } else {
            std::cout << "[TEST FM INDEX] " << hits.size() << " queries are found in " <<
                      elapsed_seconds.count() << " seconds." << std::endl;
        }

        std::cout << "[TEST FM INDEX] Check hits" << std::endl;

        if (check_hits(queries, hits, fasta)) {
            std::cout << "[TEST FM INDEX] ALL HITS FOUND CORRECTLY\n";
        } else {
            std::cerr << "[TEST FM INDEX] SOME HITS ARE WRONG!\n";
        }
    }

    {
        std::cout << "[TEST FM INDEX] Search queries with check" << std::endl;

        std::vector<hit> hits{};
        auto start = std::chrono::steady_clock::now();
        test_index.backward_search(queries, hits, true);
        auto end = std::chrono::steady_clock::now();
        std::chrono::duration<double> elapsed_seconds = end - start;

        if (hits.size() == 1) {
            std::cout << "[TEST FM INDEX] " << hits.size() << " query is found in " <<
                      elapsed_seconds.count() << " seconds." << std::endl;
        } else {
            std::cout << "[TEST FM INDEX] " << hits.size() << " queries are found in " <<
                      elapsed_seconds.count() << " seconds." << std::endl;
        }

        std::cout << "[TEST FM INDEX] Check hits" << std::endl;

        if (check_hits(queries, hits, fasta)) {
            std::cout << "[TEST FM INDEX] ALL HITS FOUND CORRECTLY\n";
        } else {
            std::cerr << "[TEST FM INDEX] SOME HITS ARE WRONG!\n";
        }
    }


    {
        size_t number_of_aaa = 3;
        std::cout << "[TEST FM INDEX] Search queries with up to " << number_of_aaa <<
                        " ambiguous amino acids without check" << std::endl;

        std::vector<struct hit> hits{};
        auto start = std::chrono::steady_clock::now();
        test_index.backward_search(queries, hits, false, number_of_aaa);
        auto end = std::chrono::steady_clock::now();
        std::chrono::duration<double> elapsed_seconds = end - start;

        if (hits.size() == 1) {
            std::cout << "[TEST FM INDEX] " << hits.size() << " query is found in " <<
                      elapsed_seconds.count() << " seconds." << std::endl;
        } else {
            std::cout << "[TEST FM INDEX] " << hits.size() << " queries are found in " <<
                      elapsed_seconds.count() << " seconds." << std::endl;
        }

        std::cout << "[TEST FM INDEX] Check hits" << std::endl;

        if (check_hits(queries, hits, fasta)) {
            std::cout << "[TEST FM INDEX] ALL HITS FOUND CORRECTLY\n";
        } else {
            std::cerr << "[TEST FM INDEX] SOME HITS ARE WRONG!\n";
        }
    }

    {
        size_t number_of_aaa = 3;
        std::cout << "[TEST FM INDEX] Search queries with up to " << number_of_aaa <<
                  " ambiguous amino acids with check" << std::endl;

        std::vector<struct hit> hits{};
        auto start = std::chrono::steady_clock::now();
        test_index.backward_search(queries, hits, true, number_of_aaa);
        auto end = std::chrono::steady_clock::now();
        std::chrono::duration<double> elapsed_seconds = end - start;

        if (hits.size() == 1) {
            std::cout << "[TEST FM INDEX] " << hits.size() << " query is found in " <<
                      elapsed_seconds.count() << " seconds." << std::endl;
        } else {
            std::cout << "[TEST FM INDEX] " << hits.size() << " queries are found in " <<
                      elapsed_seconds.count() << " seconds." << std::endl;
        }

        std::cout << "[TEST FM INDEX] Check hits" << std::endl;

        if (check_hits(queries, hits, fasta)) {
            std::cout << "[TEST FM INDEX] ALL HITS FOUND CORRECTLY\n";
        } else {
            std::cerr << "[TEST FM INDEX] SOME HITS ARE WRONG!\n";
        }
    }
}

/*void test_search_iterative() {

    std::cout << "[TEST FM INDEX] BUILD FM INDEX FROM MULTIPLE SEQUENCES" << std::endl;
    std::cout << "[TEST FM INDEX] Generate Data" << std::endl;

    std::vector<std::string> fasta{};
    generate_fasta(1'000, 100, fasta);
    std::vector<std::string> queries{};
    generate_queries(1'000'000, 8, queries);

    std::cout << "[TEST FM INDEX] Build Index" << std::endl;

    FM_Index test_index{fasta};

    {
        std::cout << "[TEST FM INDEX] Search queries ITERATIVE" << std::endl;

        std::vector<hit> hits{};
        auto start = std::chrono::steady_clock::now();
        test_index.backward_search_it(queries, hits);
        auto end = std::chrono::steady_clock::now();
        std::chrono::duration<double> elapsed_seconds = end - start;

        if (hits.size() == 1) {
            std::cout << "[TEST FM INDEX] " << hits.size() << " query is found in " <<
                      elapsed_seconds.count() << " seconds." << std::endl;
        } else {
            std::cout << "[TEST FM INDEX] " << hits.size() << " queries are found in " <<
                      elapsed_seconds.count() << " seconds." << std::endl;
        }

        std::cout << "[TEST FM INDEX] Check hits" << std::endl;

        if (check_hits(queries, hits, fasta)) {
            std::cout << "[TEST FM INDEX] ALL HITS FOUND CORRECTLY\n";
        } else {
            std::cerr << "[TEST FM INDEX] SOME HITS ARE WRONG!\n";
        }
    }

    {
        size_t number_of_aaa = 3;
        std::cout << "[TEST FM INDEX] Search ITERATIVE queries with up to " << number_of_aaa <<
                  " ambiguous amino acids" << std::endl;

        std::vector<struct hit> hits{};
        auto start = std::chrono::steady_clock::now();
        test_index.backward_search_it(queries, hits, number_of_aaa);
        auto end = std::chrono::steady_clock::now();
        std::chrono::duration<double> elapsed_seconds = end - start;

        if (hits.size() == 1) {
            std::cout << "[TEST FM INDEX] " << hits.size() << " query is found in " <<
                      elapsed_seconds.count() << " seconds." << std::endl;
        } else {
            std::cout << "[TEST FM INDEX] " << hits.size() << " queries are found in " <<
                      elapsed_seconds.count() << " seconds." << std::endl;
        }

        std::cout << "[TEST FM INDEX] Check hits" << std::endl;

        if (check_hits(queries, hits, fasta)) {
            std::cout << "[TEST FM INDEX] ALL HITS FOUND CORRECTLY\n";
        } else {
            std::cerr << "[TEST FM INDEX] SOME HITS ARE WRONG!\n";
        }
    }
}*/
