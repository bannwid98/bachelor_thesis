#include "suffix_array.h"

Suffix_Array::Suffix_Array(const std::vector<std::string> &fasta) {

    if (fasta.empty()) std::cerr << "[SUFFIX ARRAY] Insert empty fast file!" << std::endl;
    if (fasta.size() > std::numeric_limits<uint32_t>::max()) std::cerr << "[SUFFIX ARRAY] To many sequences!" << std::endl;

    size_t n = 0;// number of suffixes

    std::vector<uint32_t> cumulated_sizes{};
    cumulated_sizes.reserve(fasta.size()+1);
    cumulated_sizes.push_back(0);

    // Get the number of suffixes
    for (const std::string &sequence : fasta) {

        if (sequence.size() > std::numeric_limits<uint16_t>::max()) std::cerr << "[SUFFIX ARRAY] Sequence over size!"  << std::endl;

        n += sequence.size()+1;
        cumulated_sizes.push_back(n);
    }

    if (n > std::numeric_limits<uint32_t>::max()) std::cerr << "[SUFFIX ARRAY] Number of suffixes is over size!"  << std::endl;

    std::vector<struct suffix> suffixes{};
    suffixes.reserve(n);

    // Get all suffixes
    for (size_t j = 0; j < fasta.size(); ++j) {

        const std::string &sequence = fasta[j];

        struct suffix last_suffix{};

        last_suffix.index = std::make_pair(static_cast<uint32_t>(j), static_cast<uint16_t>(sequence.size()));
        last_suffix.ranks = std::make_pair(0, 0);

        suffixes.push_back(last_suffix);

        for (size_t i = 0; i < sequence.size(); ++i) {

            struct suffix s{};

            s.index = std::make_pair(static_cast<uint32_t>(j), static_cast<uint16_t>(i));
            s.ranks = std::make_pair(to_int(sequence[i])+1,
                                     ((i + 1) < sequence.size())? to_int(sequence[i + 1])+1: 1);

            suffixes.push_back(s);
        } // for (size_t i = 0; i < sequence.size(); ++i)
    } // for (size_t j = 0; j < fasta.size(); ++j)

    // Sort all suffixes according to the first to characters
    std::sort(suffixes.begin(), suffixes.end());

    std::vector<size_t> reverse_sa{};
    reverse_sa.resize(n);

    /*{
            auto get_index =[=](const size_t i) {
                return (cumulated_sizes[suffixes[i].index.first] + suffixes[i].index.second);
            };

            auto get_size =[=](const size_t i) {
                return (cumulated_sizes[suffixes[i].index.first + 1] - cumulated_sizes[suffixes[i].index.first]);
            };
    }*/

    // Then for the first 4, first 8 and so on...
    for (size_t k = 4; k < 2*n; k *= 2) {

        // First assigning new first ranks to the suffixes
        uint32_t first_rank = 0;
        uint32_t previous_rank = suffixes[0].ranks.first;

        suffixes[0].ranks.first = first_rank;
        reverse_sa[(cumulated_sizes[suffixes[0].index.first] + suffixes[0].index.second)] = 0;

        uint32_t count = 0;
        std::vector<uint32_t> splits{};
        splits.push_back(count);

        for (size_t i = 1; i < n; ++i) {

            ++count;

            // CHECK: if ranks equals the previous ranks
            if (suffixes[i].ranks.first == previous_rank && suffixes[i].ranks.second == suffixes[i-1].ranks.second) {

                // IF: assign the same first first_rank as the previous one
                previous_rank = suffixes[i].ranks.first;
                suffixes[i].ranks.first = first_rank;
            } else {

                // ELSE: increment the first first_rank
                splits.push_back(count);
                previous_rank = suffixes[i].ranks.first;
                suffixes[i].ranks.first = ++first_rank;
            }
            reverse_sa[(cumulated_sizes[suffixes[i].index.first] + suffixes[i].index.second)] = i;
        } // for (size_t i = 1; i < n; ++i)

        // Then store new second ranks
        for (size_t i = 0; i < n; ++i) {

            uint32_t next_index = suffixes[i].index.second + k/2;

            // WHY DOSE NOT WORK GET_SIZE?!?!?!
            if (next_index < (cumulated_sizes[suffixes[i].index.first + 1] -
                        cumulated_sizes[suffixes[i].index.first])) {
                suffixes[i].ranks.second =
                        suffixes[reverse_sa[cumulated_sizes[suffixes[i].index.first] + next_index]].ranks.first;
            } else {
                suffixes[i].ranks.second = 0;
            }
        } // for (size_t i = 0; i < n; ++i)

        splits.push_back(n);

        if (splits.size() > std::numeric_limits<uint32_t>::max()) std::cerr << "[SUFFIX ARRAY] First rank overflow!"  << std::endl;

        //if (k == 4) {
            #pragma omp parallel for num_threads(8)  default(none) shared(splits, suffixes)
            for (size_t i = 1; i < splits.size(); ++i) {
                sort((suffixes.begin() + splits[i - 1]), (suffixes.begin() + splits[i]),
                     [](const suffix &lhs, const suffix &rhs) { return lhs.ranks.second < rhs.ranks.second; });
            }

            for (size_t i = 1; i < suffixes.size(); ++i){
                if (suffixes[i] < suffixes[i-1]) std::cerr << "[ERROR] " << i << std::endl;
            }

        /*} else {
            std::sort(suffixes.begin(), suffixes.end());
        }*/

    } // for (size_t k = 4; k < 2*n; k *= 2)

    // Save the suffix array
    this->sa.reserve(n);

    for (struct suffix &s : suffixes) {
        sa.push_back(s.index);
    }
}

std::vector<std::pair<uint32_t, uint16_t>> Suffix_Array::get_sa() {
    return this->sa;
}
