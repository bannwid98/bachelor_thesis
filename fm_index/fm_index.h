#ifndef IDEX_FM_INDEX_H
#define IDEX_FM_INDEX_H

#include <vector>
#include <array>
#include <string>
#include <cstring>
#include <iterator>

#include <algorithm>
#include <memory>
#include <fstream>
#include <cmath>
#include <iostream>
#include <cstdint>

#include <map>
#include <unordered_map>

#include "../suffix_array/suffix_array.h"

#pragma once

struct hit {
    size_t query{};
    std::vector<std::pair<uint32_t, uint16_t>> hit_vec{};
};

class FM_Index {

private:
    static constexpr size_t alphabet_size{27};
    size_t occ_mat_compression = 512;
    size_t sa_compression = 32;

    //std::map<size_t, std::array<size_t, alphabet_size>> occ_map{};
    //std::unordered_map<size_t, std::array<size_t, alphabet_size>> occ_map{};

    std::unordered_map<size_t, std::pair<uint32_t, uint16_t>> sa_map{};

    //std::vector<std::pair<uint32_t, uint16_t>> suffix_array{};
    std::array<size_t, alphabet_size> cumulated_count{}; // Compressed first column
    std::string bwt{}; // Last column
    std::vector<std::array<uint32_t, alphabet_size>> occurrence{};

    static std::array<int, alphabet_size> is_ambiguous_arr;

    static constexpr char to_int(const char character) noexcept {
        return  static_cast<char>(character - 64);
    }

    static constexpr char to_char(const char amino_acid) noexcept {
        return  static_cast<char>(amino_acid + 64);
    }

    bool backward_search(const std::string &query, size_t &lower_bound, size_t &upper_bound);
    void backward_search(const std::string &query, size_t lower_bound, size_t upper_bound, int position,
                         std::vector<size_t> &lower_boundaries, std::vector<size_t> &upper_boundaries, size_t count_aaa);
    void backward_search(const std::string &query, size_t lower_bound, size_t upper_bound, int position,
                                    std::vector<size_t> &lower_boundaries, std::vector<size_t> &upper_boundaries);

    std::array<uint32_t , alphabet_size> get_row(size_t row_num) const;
    std::pair<uint32_t, uint16_t> get_suffix(size_t position) const;

    std::vector<std::pair<uint8_t, uint8_t>>  compress_bwt();
    static std::string decompress_bwt(const std::vector<std::pair<uint8_t, uint8_t>> &compressed_bwt, size_t bwt_size);

public:
    // RULE OF SIX
    FM_Index() = default;
    FM_Index(FM_Index const &) = default;
    FM_Index(FM_Index &&) = default;
    FM_Index & operator=(FM_Index const &) = default;
    FM_Index & operator=(FM_Index &&) = default;
    ~FM_Index() = default;

    bool operator==(const FM_Index &) const;
    //bool operator==(FM_Index const &) const = default;

    // CUSTOM CONSTRUCTOR
    explicit FM_Index(const std::vector<std::string> &fasta);

    bool backward_search(const std::vector<std::string> &queries, std::vector<struct hit> &hits, bool check, size_t number_of_aaa = 0);
    bool backward_search(const std::string &query, struct hit &hits, bool check, size_t number_of_aaa = 0);

    bool write_index(const std::string & file_name, bool bwt_compressed = false);
    bool load_index(const std::string & file_name, bool bwt_compressed = false);

    [[nodiscard]] std::string get_bwt() const;
    [[nodiscard]] std::array<size_t, alphabet_size> get_cumulated_count() const;
    [[nodiscard]] std::vector<std::array<uint32_t , alphabet_size>> get_occurrence_matrix() const;
    [[nodiscard]] std::vector<std::pair<uint32_t, uint16_t>> get_suffix_array() const;

    void recompress_occ_mat(size_t rate);

    //bool backward_search_it(const std::string &query, struct hit &hits, size_t number_of_aaa = 0);
    //bool backward_search_it (const std::vector<std::string> &queries, std::vector<struct hit> &hits,
    //                                                              const size_t number_of_aaa = 0);
};

#endif //IDEX_FM_INDEX_H
