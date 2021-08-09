#include "suffix_array/suffix_array.h"

void generate_fasta(const size_t fsize, std::vector<std::string> & fasta) {

    for (int j = 0; j < fsize; ++j) {
        size_t length = ((random() % 1'000) + 500);
        std::string sequence{};
        sequence.reserve(length);

        for (size_t i = 0; i < length; ++i)
            sequence.push_back(static_cast<char>((random() % 25) + 65));

        //sequence.push_back('\n');
        fasta.push_back(sequence);
    }
}


void test_build_array() {


    Suffix_Array my_mini_test_sa{mini_test_fasta};

    if (my_mini_test_sa.get_sa() == mini_test_sa) std::cout << "Passed mini test" << std::endl;

    std::cout << "Generate Data" << std::endl;

    std::vector<std::string> test_fasta{};
    std::size_t fasta_size = 10;
    generate_fasta(fasta_size,test_fasta);

    std::cout << "Build suffix array" << std::endl;
    Suffix_Array my_test_sa{test_fasta};

    std::cout << "Check if suffix are sorted" << std::endl;

    std::vector<std::pair<uint32_t, uint16_t>> suffix_sa = my_test_sa.get_sa();
    for (size_t i = 1; i < suffix_sa.size(); ++i) {

        auto &suffix = suffix_sa[i];
        auto &previous_suffix = suffix_sa[i-1];

        std::string &fasta_of_suffix = test_fasta[suffix.first];
        std::string &fasta_of_previous_suffix = test_fasta[previous_suffix.first];

        size_t suffix_size = fasta_of_suffix.size() - suffix.second;
        size_t previous_suffix_size = fasta_of_previous_suffix.size() - previous_suffix.second;

        if (fasta_of_suffix.substr(suffix.second, suffix_size) <
            fasta_of_previous_suffix.substr(previous_suffix.second, previous_suffix_size)) {
            std::cerr << "[TEST BUILD ARRAY] Suffix at position " << i << " is smaller then the previous one" << std::endl;
        }
    }
}