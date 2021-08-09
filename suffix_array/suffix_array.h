#ifndef BACHELORARBEIT_SUFFIX_ARRAY_H
#define BACHELORARBEIT_SUFFIX_ARRAY_H

#include <vector>
#include <string>
#include <utility> // std::pair, std::make_pair
#include <limits> // std::numeric_limits
#include <algorithm>

//#include <omp.h>
#include <iostream>

class Suffix_Array {
private:
    std::vector<std::pair<uint32_t , uint16_t>> sa{}; // up to 4,294,967,295 and 65,535

    static constexpr uint8_t to_int(const char character) noexcept {
        return static_cast<uint8_t>(character - 64);
    }

    struct suffix {
        std::pair<uint32_t, uint16_t> index{}; // up to 4,294,967,295 and 65,535
        std::pair<uint32_t, uint32_t> ranks{}; //

        inline bool operator<(struct suffix const &rh_suffix) const {
            if (this->ranks.first == rh_suffix.ranks.first) {
                return this->ranks.second < rh_suffix.ranks.second;
            } else {
                return this->ranks.first < rh_suffix.ranks.first;
            }
        }

        inline bool operator==(struct suffix const &rh_suffix) const {
            return ((this->ranks.first == rh_suffix.ranks.first) &&
                    (this->ranks.second == rh_suffix.ranks.second));
        }
    };

public:
    // RULE OF SIX
    Suffix_Array() = delete;

    Suffix_Array(Suffix_Array const &) = default;
    Suffix_Array(Suffix_Array &&) = default;

    Suffix_Array & operator=(Suffix_Array const &) = default;
    Suffix_Array & operator=(Suffix_Array &&) = default;

    ~Suffix_Array() = default;

    // CUSTOM CONSTRUCTOR
    explicit Suffix_Array(const std::vector<std::string> &fasta);

    std::vector<std::pair<uint32_t, uint16_t>> get_sa();
};

#endif //BACHELORARBEIT_SUFFIX_ARRAY_H
