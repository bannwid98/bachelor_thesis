#include "fm_index.h"

std::array<int, FM_Index::alphabet_size> FM_Index::is_ambiguous_arr{0, 0, 0, 0, 2, 26,0,0,0,
                                                                 10,0,0,10,0,2,0,0,26,
                                                                 0,0,0,0,0,0,0,0,0 };

bool FM_Index::backward_search(const std::vector<std::string> &queries, std::vector<struct hit> &hits, const bool check, const size_t number_of_aaa) {

    if (queries.empty()) {
        std::cout << "[BACKWARD SEARCH] Input queries are empty\n";
        return true;
    }

    if (this->bwt.empty()) {
        std::cout << "[BACKWARD SEARCH] First construct index\n";
        return false;
    }

    if (!hits.empty()) {
        std::cout << "[BACKWARD SEARCH] Input vector already contain hit\n";
    }

    for (size_t i = 0; i < queries.size(); ++i) {

        struct hit findings{};
        findings.query = i;

        if (backward_search(queries[i], findings, check, number_of_aaa)) {
            hits.push_back(findings);
        }
    }

    return !hits.empty();
}

// Initialize recursion
bool FM_Index::backward_search(const std::string &input_query, struct hit &hits, const bool check, const size_t number_of_aaa) {

    std::string query{};
    query.reserve(input_query.size());

    if (input_query.empty()) {
        std::cout << "[BACKWARD SEARCH] Input query is empty\n";
        return true;
    } else {
        for (const char character : input_query) {
            query.push_back(to_int(character));
        }
    }

    if (number_of_aaa) {
        std::vector<size_t> lower_boundaries{};
        std::vector<size_t> upper_boundaries{};

        lower_boundaries.reserve(number_of_aaa);
        upper_boundaries.reserve(number_of_aaa);

        char current_aa = query.back();

        this->backward_search(query,
                              this->cumulated_count[current_aa - 1], // lower bound
                              this->cumulated_count[current_aa] - 1, // upper bound
                              static_cast<int>(query.size()) - 2, // position
                              lower_boundaries, upper_boundaries, number_of_aaa);

        int is_first_aa_ambiguous = FM_Index::is_ambiguous_arr[current_aa];

        if (is_first_aa_ambiguous) {
            backward_search(query,
                            this->cumulated_count[is_first_aa_ambiguous - 1], // lower bound
                            this->cumulated_count[is_first_aa_ambiguous] - 1, // upper bound
                            static_cast<int>(query.size()) - 2, // position
                            lower_boundaries, upper_boundaries, number_of_aaa - 1);
        }

        if (current_aa != 24 && this->cumulated_count[23] < this->cumulated_count[24]) {
            backward_search(query,
                            this->cumulated_count[23], // lower bound
                            this->cumulated_count[24] - 1, // upper bound
                            static_cast<int>(query.size()) - 2, // position
                            lower_boundaries, upper_boundaries, number_of_aaa - 1);
        }
        if (check) {
            for (size_t i = 0; i < lower_boundaries.size(); ++i) {

                size_t lower_bound = lower_boundaries[i];
                size_t upper_bound = upper_boundaries[i];

                size_t lower_bound_K = this->get_row(lower_bound)[11] +
                                       this->cumulated_count[10];

                size_t upper_bound_K = this->get_row(upper_bound + 1)[11] +
                                       this->cumulated_count[10] - 1;

                for (size_t j = lower_bound_K; j <= upper_bound_K; ++j) {

                    std::pair<uint32_t, uint16_t> suffix = this->get_suffix(j);
                    ++suffix.second;

                    hits.hit_vec.push_back(suffix);
                }

                size_t lower_bound_R = this->get_row(lower_bound)[18] +
                                       this->cumulated_count[17];

                size_t upper_bound_R = this->get_row(upper_bound + 1)[18] +
                                       this->cumulated_count[17] - 1;

                for (size_t j = lower_bound_R; j <= upper_bound_R; ++j) {
                    std::pair<uint32_t, uint16_t> suffix = this->get_suffix(j);
                    ++suffix.second;

                    hits.hit_vec.push_back(suffix);
                }
            }
        } else {
            for (size_t i = 0; i < lower_boundaries.size(); ++i) {

                size_t lower_bound = lower_boundaries[i];
                size_t upper_bound = upper_boundaries[i];

                for (size_t j = lower_bound; j <= upper_bound; ++j) {
                    hits.hit_vec.push_back(this->get_suffix(j));
                }
            }
        }

        return !hits.hit_vec.empty();

    } else {

        // Backward search without ambiguous amino acids
        size_t lower_bound{}, upper_bound{};

        if (this->backward_search(query, lower_bound, upper_bound)) {

            if (check) {
                size_t lower_bound_K = this->get_row(lower_bound)[11] +
                                       this->cumulated_count[10];

                size_t upper_bound_K = this->get_row(upper_bound + 1)[11] +
                                       this->cumulated_count[10] - 1;

                for (size_t j = lower_bound_K; j <= upper_bound_K; ++j) {
                    std::pair<uint32_t, uint16_t> suffix = this->get_suffix(j);
                    ++suffix.second;

                    hits.hit_vec.push_back(suffix);
                }

                size_t lower_bound_R = this->get_row(lower_bound)[18] +
                                       this->cumulated_count[17];

                size_t upper_bound_R = this->get_row(upper_bound + 1)[18] +
                                       this->cumulated_count[17] - 1;

                for (size_t j = lower_bound_R; j <= upper_bound_R; ++j) {
                    std::pair<uint32_t, uint16_t> suffix = this->get_suffix(j);
                    ++suffix.second;

                    hits.hit_vec.push_back(suffix);
                }
            } else {

                hits.hit_vec.reserve(upper_bound - lower_bound + 1);

                for (size_t i = lower_bound; i <= upper_bound; ++i) {

                    hits.hit_vec.push_back(this->get_suffix(i));
                }
            }
            return !hits.hit_vec.empty();

        } else {
            return false;
        }

    }
}

bool FM_Index::backward_search(const std::string &query,
                               size_t &lower_bound, size_t &upper_bound) {

    // Initialize boundaries
    lower_bound = this->cumulated_count[query[query.size() - 1] - 1];
    upper_bound = this->cumulated_count[query[query.size() - 1]] - 1;

    if (lower_bound > upper_bound) return false;

    for (int i = static_cast<int>(query.size()) - 2; i >= 0; --i) {

        // Reset lower boundary
        lower_bound = this->get_row(lower_bound)[query[i]] +
                      this->cumulated_count[query[i] - 1];

        upper_bound = this->get_row(upper_bound + 1)[query[i]] +
                      this->cumulated_count[query[i] - 1] - 1;

        if (lower_bound > upper_bound) return false;
    }

    return true;
}

// Recursion body
void FM_Index::backward_search(const std::string &query, size_t lower_bound, size_t upper_bound, int position,
                               std::vector<size_t> &lower_boundaries, std::vector<size_t> &upper_boundaries, size_t count_aaa) {

    if (lower_bound > upper_bound) return;

    if (position >= 0) {

        char current_aa = query[position];

        if (count_aaa) {
            backward_search(query,
                            this->get_row(lower_bound)[current_aa] +
                            this->cumulated_count[current_aa - 1], // lower bound
                            this->get_row(upper_bound + 1)[current_aa] +
                            this->cumulated_count[current_aa - 1] - 1, // upper bound
                            position - 1, lower_boundaries, upper_boundaries, count_aaa);

            int is_aa_ambiguous = FM_Index::is_ambiguous_arr[static_cast<int>(static_cast<u_char>(current_aa))];


            if (is_aa_ambiguous) {
                backward_search(query,
                                this->get_row(lower_bound)[is_aa_ambiguous] +
                                this->cumulated_count[is_aa_ambiguous - 1], // lower bound
                                this->get_row(upper_bound + 1)[is_aa_ambiguous] +
                                this->cumulated_count[is_aa_ambiguous - 1] - 1, // upper bound
                                position - 1, lower_boundaries, upper_boundaries, count_aaa - 1);
            }

            if (current_aa != 24 && this->get_row(lower_bound)[24] < this->get_row(upper_bound + 1)[24]) {
                backward_search(query,
                                this->get_row(lower_bound)[24] +
                                this->cumulated_count[23], // lower bound
                                this->get_row(upper_bound + 1)[24] +
                                this->cumulated_count[23] - 1, // upper bound
                                position - 1, lower_boundaries, upper_boundaries, count_aaa - 1);
            }
        } else { // if (count_aaa)
            backward_search(query,
                            this->get_row(lower_bound)[current_aa] +
                            this->cumulated_count[current_aa - 1], // lower bound
                            this->get_row(upper_bound + 1)[current_aa] +
                            this->cumulated_count[current_aa - 1] - 1, // upper bound
                            position - 1, lower_boundaries, upper_boundaries);
        }
    } else { // if (position >= 0)
        lower_boundaries.push_back(lower_bound);
        upper_boundaries.push_back(upper_bound);
        return;
    }
}

void FM_Index::backward_search(const std::string &query, size_t lower_bound, size_t upper_bound, int position,
                               std::vector<size_t> &lower_boundaries, std::vector<size_t> &upper_boundaries) {

    if (lower_bound > upper_bound)  {
        return;
    } else {
        if (position < 0) {
            lower_boundaries.push_back(lower_bound);
            upper_boundaries.push_back(upper_bound);
            return;
        } else {
            backward_search(query,
                            this->get_row(lower_bound)[query[position]] +
                            this->cumulated_count[query[position] - 1],
                            this->get_row(upper_bound + 1)[query[position]] +
                            this->cumulated_count[query[position] - 1] - 1,
                            position - 1, lower_boundaries, upper_boundaries);
        }
    }
}


FM_Index::FM_Index(const std::vector<std::string> &fasta) {

    if (fasta.empty()) {
        std::cout << "[FM INDEX] Insert empty fasta file\n";
        return;
    }

    // Get suffix array
    std::vector<std::pair<uint32_t, uint16_t>> suffix_array = Suffix_Array{fasta}.get_sa();

    size_t size = suffix_array.size();
    size_t n = 0;

    this->bwt.reserve(size);
    this->occurrence.reserve(std::ceil(size/this->occ_mat_compression));

    std::array<size_t, 27> counter{};
    std::array<uint32_t, 27> occurrence_counter{0};
    char character;

    //this->occ_map[0] = occurrence_counter;
    this->occurrence.push_back(occurrence_counter);

    for (size_t i = 0; i < suffix_array.size(); ++i)    {
        const std::pair<uint32_t, uint16_t> suffix = suffix_array[i];

        if (suffix.second == 0) {
            character = 0;
        } else {
            character = to_int(fasta[suffix.first][suffix.second - 1]);
        }

        // Get BWT
        this->bwt.push_back(character);

        ++counter[character];
        ++occurrence_counter[character];
        ++n;

        if (!(suffix.second % this->sa_compression)) {
            this->sa_map[i] = suffix;
        }

        if (n == this->occ_mat_compression) {

            // Get occurrence matrix

            //this->occ_map[i+1] = occurrence_counter;

            this->occurrence.push_back(occurrence_counter);
            n = 0;
        }
    }

    if (n > this->occ_mat_compression/2) this->occurrence.push_back(occurrence_counter);

    for (size_t i = 1; i < counter.size(); ++i) {
        counter[i] = counter[i-1] + counter[i];
    }

    // Get cumulated count array
    this->cumulated_count = counter;
}

std::pair<uint32_t, uint16_t> FM_Index::get_suffix(const size_t position) const {

    auto search = this->sa_map.find(position);

    if(search != this->sa_map.end()) {
    //if (this->sa_map.contains(position)) {
        return this->sa_map.at(position);
    } else {

        size_t counter = 0;
        size_t current_position{position};

        do {

            current_position = this->get_row(current_position)[this->bwt[current_position]] +
                   this->cumulated_count[this->bwt[current_position] - 1];
            ++counter;
            search = this->sa_map.find(current_position);

        } while (search == this->sa_map.end());
        //} while (!this->sa_map.contains(current_position));

        std::pair<uint32_t, uint16_t> suffix = this->sa_map.at(current_position);

        suffix.second += counter;

        return suffix;
    }
}

std::array<uint32_t , 27> FM_Index::get_row(const size_t row_num) const {

    if (row_num == 5'655'015'511) {
        std::cout << "STOP";
    }


    std::div_t dv{std::div(static_cast<int>(row_num), static_cast<int>(this->occ_mat_compression))};

    if (!dv.rem) {
        return this->occurrence[dv.quot];
    } else {

        if (dv.rem > this->occ_mat_compression/2) {

            std::array<uint32_t, 27> row{this->occurrence[dv.quot+1]};

            for (size_t i = (((dv.quot+1) * this->occ_mat_compression) < bwt.size())?
                                    ((dv.quot+1) * this->occ_mat_compression - 1) : (bwt.size()-2);
                                                i >= row_num; --i) {

                --row[bwt[i]];
            }

            return row;

        } else {
            std::array<uint32_t, 27> row{this->occurrence[dv.quot]};

            for (size_t i = dv.quot * this->occ_mat_compression;
                 i < row_num; ++i) {
                ++row[bwt[i]];
            }

            return row;
        }
    }
}

std::string FM_Index::get_bwt() const {

    std::string out_bwt{};
    out_bwt.reserve(this->bwt.size());

    for (const char aa : this->bwt) {
        out_bwt.push_back(to_char(aa));
    }

    return out_bwt;
}

std::array<size_t, 27> FM_Index::get_cumulated_count() const {
    return this->cumulated_count;
}

std::vector<std::array<uint32_t , 27>> FM_Index::get_occurrence_matrix() const {
    return this->occurrence;
}

std::vector<std::pair<uint32_t, uint16_t>> FM_Index::get_suffix_array() const {

    std::vector<std::pair<uint32_t, uint16_t>> suffix_array{};
    suffix_array.reserve(this->bwt.size());

    for (size_t i = 0; i < bwt.size(); ++i) {

        suffix_array.push_back(this->get_suffix(i));
    }

    return suffix_array;
}

std::vector<std::pair<uint8_t, uint8_t>> FM_Index::compress_bwt() {

    std::vector<std::pair<uint8_t, uint8_t>> compressed_bwt{};
    compressed_bwt.reserve(this->bwt.size() / 128);

    for (size_t i = 0; i < bwt.size(); ) {

        uint8_t aa = this->bwt[i];
        uint8_t counter = 1; ++i;

        while (counter < 255 && i < bwt.size() && aa == this->bwt[i]) {
            ++counter;
            ++i;
        }

        compressed_bwt.emplace_back(aa, counter);
    }

    return compressed_bwt;
}

std::string FM_Index::decompress_bwt(const std::vector<std::pair<uint8_t, uint8_t>> &compressed_bwt, const size_t bwt_size) {

    std::string uncompressed_bwt{};
    uncompressed_bwt.resize(bwt_size);

    size_t position = 0;

    for (const std::pair<uint8_t, uint8_t> &entry : compressed_bwt) {

        memset (&uncompressed_bwt[position], entry.first, entry.second);
        position += entry.second;
    }

    return uncompressed_bwt;
}

bool FM_Index::write_index(const std::string &file_name, const bool bwt_compressed) {

    // Initialize output stream
    std::ofstream fout{file_name, std::ios::binary | std::ios::out};

    // CHECK: if output stream is open
    if (!fout.is_open()) {
        std::cerr << "[WRITE INDEX] Can't write file: " << file_name << "\n";
        return false;
    }

    // Write compression rates
    {
        fout.write(reinterpret_cast<char *>(&(this->occ_mat_compression)), sizeof(this->occ_mat_compression));
        fout.write(reinterpret_cast<char *>(&(this->sa_compression)), sizeof(this->sa_compression));
    }

    // Write suffix array
    {
        auto size = static_cast<std::streamsize>(this->sa_map.size());
        fout.write(reinterpret_cast<char *>(&size), sizeof(size));

        for (std::pair<size_t, std::pair<uint32_t, uint16_t>> entry : this->sa_map) {

            fout.write(reinterpret_cast<char *>(&entry.first), sizeof(entry.first));
            fout.write(reinterpret_cast<char *>(&entry.second.first), sizeof(entry.second.first));
            fout.write(reinterpret_cast<char *>(&entry.second.second), sizeof(entry.second.second));
        }
    }

    // Write BWT
    if (bwt_compressed) {
        std::vector<std::pair<uint8_t, uint8_t>> compressed_bwt = this->compress_bwt();

        auto size = static_cast<std::streamsize>(this->bwt.size());
        fout.write(reinterpret_cast<char *>(&size), sizeof(size));

        size = static_cast<std::streamsize>(compressed_bwt.size());
        fout.write(reinterpret_cast<char *>(&size), sizeof(size));

        for (std::pair<uint8_t, uint8_t> entry : compressed_bwt) {
            fout.write(reinterpret_cast<char *>(&entry.first), sizeof(entry.first));
            fout.write(reinterpret_cast<char *>(&entry.second), sizeof(entry.second));
        }
    } else {
        auto size = static_cast<std::streamsize>(this->bwt.size());
        fout.write(reinterpret_cast<char *>(&size), sizeof(size));

        fout.write(reinterpret_cast<char *>(&bwt[0]), size);
    }

    // Write occurrence matrix
    {
        // First write number of rows
        auto size = static_cast<std::streamsize>(this->occurrence.size());
        fout.write(reinterpret_cast<char *>(&size), sizeof(size));

        size = 27; // Alphabet size

        for (const std::array<uint32_t , 27> &row : this->occurrence) {

            std::ostream_iterator<size_t> ostreamIterator{fout, " "};
            std::copy(row.begin(), row.end(), ostreamIterator);
        }
    }

    // Write count array
    {
        std::ostream_iterator<size_t> ostreamIterator{fout, " "};
        std::copy(this->cumulated_count.begin(), this->cumulated_count.end(), ostreamIterator);
    }

    // Close the output stream
    fout.close();

    return true;
}

bool FM_Index::load_index(const std::string &file_name,  const bool bwt_compressed) {

    // Initialize input stream
    std::ifstream  fin{file_name, std::ios::binary | std::ios::in};

    // CHECK: if its open
    if (!fin.is_open()) {
        std::cerr << "[WRITE INDEX] Can't read file: " << file_name << "\n";
        return false;
    }

    std::streamsize size{};

    // Read compression rates
    {
        fin.read(reinterpret_cast<char*>(&(this->occ_mat_compression)), sizeof(this->occ_mat_compression));
        fin.read(reinterpret_cast<char*>(&(this->sa_compression)), sizeof(this->sa_compression));
    }

    // Read suffix array
    {
        std::streamsize sa_size{};
        fin.read(reinterpret_cast<char *>(&sa_size), sizeof(sa_size));

        this->sa_map.reserve(sa_size);

        for (size_t i = 0; i < sa_size; ++i) {

            size_t position{};
            std::pair<uint32_t, uint16_t> suffix{};

            fin.read(reinterpret_cast<char *>(&position), sizeof(position));
            fin.read(reinterpret_cast<char *>(&suffix.first), sizeof(suffix.first));
            fin.read(reinterpret_cast<char *>(&suffix.second), sizeof(suffix.second));

            this->sa_map[position] = suffix;
        }
    }

    // Read BWT
    if (bwt_compressed) {

        std::vector<std::pair<uint8_t, uint8_t>> compressed_bwt{};
        std::streamsize bwt_size{};

        {
            fin.read(reinterpret_cast<char *>(&bwt_size), sizeof(bwt_size));

            fin.read(reinterpret_cast<char *>(&size), sizeof(size));

            compressed_bwt.resize(size);

            for (std::pair<uint8_t, uint8_t> &entry : compressed_bwt) {

                fin.read(reinterpret_cast<char *>(&entry.first), sizeof(entry.first));
                fin.read(reinterpret_cast<char *>(&entry.second), sizeof(entry.second));
            }
        }

        this->bwt = decompress_bwt(compressed_bwt, bwt_size);
    } else {
        std::streamsize bwt_size{};

        fin.read(reinterpret_cast<char *>(&bwt_size), sizeof(bwt_size));

        this->bwt.resize(bwt_size);

        fin.read(reinterpret_cast<char *>(&bwt[0]), bwt_size);
    }


    // Read occurrence matrix
    {
        // First read number of rows
        std::streamsize mat_size{};
        fin.read(reinterpret_cast<char *>(&mat_size), sizeof(mat_size));

        this->occurrence.resize(mat_size);

        for (std::array<uint32_t , 27> &row : this->occurrence) {

            std::istream_iterator<size_t> istreamIterator{fin};
            std::copy_n(istreamIterator, 27, row.begin());
        }
    }

    // Read count array
    {
        std::istream_iterator<size_t> istreamIterator{fin};
        std::copy_n(istreamIterator, 27, this->cumulated_count.begin());
    }

    // Close the input stream
    fin.close();

    return true;
}

bool FM_Index::operator==(const FM_Index &rh_fm_index) const {
    return (this->occ_mat_compression == rh_fm_index.occ_mat_compression &&
            this->sa_compression == rh_fm_index.sa_compression &&
            this->sa_map == rh_fm_index.sa_map &&
            this->bwt == rh_fm_index.bwt &&
            this->cumulated_count == rh_fm_index.cumulated_count &&
            this->occurrence == rh_fm_index.occurrence);
}

void FM_Index::recompress_occ_mat(size_t rate) {

    if (this->occ_mat_compression == rate) return;

    std::vector<std::array<uint32_t, alphabet_size>> new_occ_mat{};

    std::array<uint32_t, alphabet_size> occurrence_counter{0};

    new_occ_mat.push_back(occurrence_counter);

    size_t n = 0;

    for (char character : this->bwt) {

        ++occurrence_counter[character];
        ++n;

        if (n == rate) {

            new_occ_mat.push_back(occurrence_counter);
            n = 0;
        }
    }

    this->occ_mat_compression = rate;
    this->occurrence = new_occ_mat;
}