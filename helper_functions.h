#include <vector>
#include <string>
#include <iostream>

constexpr std::array<int, 27> is_ambiguous_arr{0, 0, 0, 0, 2, 26,0,0,0,
                                               10,0,0,10,0,2,0,0,26,
                                               0,0,0,0,0,0,0,0,0 };

constexpr std::array<int, 27> to_int_arr{0,1,23,2,17,19,3,4,5,
                                         21,25,6,22,7,18,8,9,20,
                                         10,11,12,13,14,15,26,16,24};

constexpr std::array<char, 22> to_char{'A', 'C', 'D', 'E', 'F', 'G', 'H', 'I', 'K', 'L', 'M', 'N', 'O', 'P', 'Q', 'R', 'S', 'T', 'U', 'V', 'W', 'Y'};

constexpr int to_int(const char character) noexcept {
    return static_cast<int>(character) - 64;
}

// Checked if suffix array is correctly sorted
bool is_sorted(const std::vector<std::string> &fasta, std::vector<std::pair<uint32_t, uint16_t>> &suffix_array) {
    bool is_sorted = true;

    for (size_t i = 1; i < suffix_array.size(); ++i) {

        if (i == 972) {
            std::cout << "STPO";
        }

        std::pair<uint32_t, uint16_t> &suffix = suffix_array[i];
        std::pair<uint32_t, uint16_t> &previous_suffix = suffix_array[i-1];

        const std::string &sequence_of_suffix = fasta[suffix.first];
        const std::string &sequence_of_previous_suffix = fasta[previous_suffix.first];

        size_t suffix_size = sequence_of_suffix.size() - suffix.second;
        size_t previous_suffix_size = sequence_of_previous_suffix.size() - previous_suffix.second;

        const std::string suffix_seq = sequence_of_suffix.substr(suffix.second, suffix_size);
        const std::string suffix_prev_seq = sequence_of_previous_suffix.substr(previous_suffix.second, previous_suffix_size);

        if (suffix_seq <
                suffix_prev_seq) {
            std::cerr << "[IS SORTED] Suffix at position " << i <<
                            " is smaller then the previous one\n";
            is_sorted = false;
        }
    }
    return is_sorted;
}

// Checked if BWT is correctly build
bool check_bwt(const std::vector<std::string> &fasta, std::string &bwt,
               std::vector<std::pair<uint32_t, uint16_t>> &suffix_array) {
    bool is_correct = true;

    for (size_t i = 0; i < suffix_array.size(); ++i) {

       const std::pair<uint32_t, uint16_t> &suffix = suffix_array[i];

        if (0 == suffix.second) {
            if ('@' != bwt[i] ){
                std::cout << "[CHECK BWT] BWT at postion " << i <<
                                " is incorrect\n";
                is_correct = false;
            }
        } else if (fasta[suffix.first][suffix.second-1] != bwt[i]) {
            std::cout << "[CHECK BWT] BWT at postion " << i <<
                            " is incorrect\n";
            is_correct = false;
        }
    }
    return is_correct;
}

bool control_occ_mat(const std::string &bwt, const std::vector<std::array<uint32_t, 27>> &occ_mat) {

    bool is_correct = true;

    for (size_t i = 1; i < occ_mat.size(); ++i) {

        int character = to_int(bwt[i-1]);

        if (occ_mat[i][character] != (occ_mat[i-1][character] + 1)) {
            std::cout << "[CONTROL OCC MAT] Occurrence matrix at position " << i <<
                      " is wrong calculated\n";
            is_correct = false;
        }
    }
    return is_correct;

}

bool test_count_array(const std::vector<std::string> &fasta, const std::array<size_t, 27> &count_arr) {

    std::array<size_t, 27> count{0};
    bool is_correct = true;

    for (const std::string &sequence : fasta) {
        for (const char aa : sequence) {
            ++count[to_int(aa)];
        }
    }

    count[0] = fasta.size();

    for (size_t i = 1; i < count.size(); ++i) count[i] += count[i-1];

    for (size_t i = 0; i < count.size(); ++i) {

        if (count[i] != count_arr[i]) {
            std::cerr << "[TEST COUNT ARRAY] Count array is wrong at position " << i << "\n";
            is_correct = false;
        }
    }
    return is_correct;
}

bool check_hits(const std::vector<std::string> &queries, const std::vector<struct hit> &hits, const std::vector<std::string> &fasta) {

    bool no_wrong_hits = true;

    for (const struct hit &finding : hits) {

        const std::string &query{queries[finding.query]};

        for (std::pair<uint32_t, uint16_t> hit : finding.hit_vec) {

            const std::string &sequence{fasta[hit.first]};
            size_t j = hit.second;

            for (char aa : query) {

                // CHECK: if query matches fasta
                if (aa != sequence[j]) {

                    int  aaa = is_ambiguous_arr[to_int(aa)];
                    int seq = to_int(sequence[j]);

                    // IF NOT: CHECK: if amino acid is ambiguous
                    if ((is_ambiguous_arr[to_int(aa)] != to_int(sequence[j])) && (to_int(sequence[j]) != 24)) {

                            std::cerr << "[CHECK HITS]  Wrong match!\n" <<
                                      "              " << query << " dosn't match witch " <<
                                      sequence.substr(hit.second, query.size()) << "\n";
                            no_wrong_hits = false;
                    }
                }
                ++j;
            }
        }
    }
    return no_wrong_hits;
}

template<typename T>
bool is_equal(const std::vector<T> &left_handed_input, const std::vector<T> &right_handed_input) {

    if (left_handed_input.size() < right_handed_input.size()) {
        std::cout << "[IS EQUAL]    Size of left handed input is smaller then the right ones\n" <<
                     "              Size of the left handed input: " << left_handed_input.size() <<
                     " size of the right handed input " << right_handed_input.size() << "\n";
        return false;
    }

    if (left_handed_input.size() > right_handed_input.size()) {
        std::cout << "[IS EQUAL] Size of left handed input is bigger then the right ones\n" <<
                     "              Size of the left handed input: " << left_handed_input.size() <<
                     " size of the right handed input " << right_handed_input.size() << "\n";

        return false;
    }

    for (size_t i = 0; i < left_handed_input.size(); ++i) {
        if (left_handed_input[i] != right_handed_input[i]) {
            std::cout << "[IS EQUAL]    The entry of the left handed input is unequal to the right one at position: " << i << "\n";
                         //"              Left handed entry: " << left_handed_input[i] << " Right handed entry: " << right_handed_input[i] << "\n";
            return false;
        }
    }

    return true;
}