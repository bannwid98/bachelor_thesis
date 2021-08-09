#include "../fm_index/fm_index.h"
#include "../cmd_parser/cmd_parser.h"

#include <chrono>
#include <ctime>

int main(int argc, char** argv) {

    std::vector<std::pair<char, std::pair<bool, std::string>>> options{
            {'i', {false, ""}}, {'o', {false, ""}},
            {'q', {false, ""}}, {'c', {true, ""}},
            {'a', {false, ""}}, {'b', {true, ""}}
    };

    CMD_Parser parser{options};

    parser.parse(argc, argv);

    std::string index_file{parser.get_argument('i')};
    std::string query_file{parser.get_argument('q')};
    std::string output_file{ parser.get_argument('o')};

    std::vector<std::string> queries{};

    std::cout << "[SEARCH] Read queries\n";
    std::ifstream fin{query_file};

    // Check if file is open
    if (!fin.is_open()) {
        std::cerr << "[SEARCH] Can't read input query file!\n";
        return 1;
    }

    bool is_empty = (fin.get(), fin.eof());

    // Check if file is empty
    if (is_empty) {
        std::cerr << "[SEARCH] Input query file is empty!\n";
        return 1;
    }

    fin.unget();

    std::string line{};

    while (getline(fin, line)) {
        queries.push_back(line);
    }

    fin.close();

    std::cout << "[SEARCH] Load FM-Index\n";

    FM_Index fm_index{};

    if (!fm_index.load_index(index_file, parser.is_set('c'))) {
        std::cerr << "[SEARCH] Can't load FM-Index\n";
        return 1;
    }

    std::cout << "[SEARCH] Search queries\n";

    std::vector<struct hit> hits{};

    std::chrono::system_clock::time_point start = std::chrono::system_clock::now();

    size_t aaa = 0;

    if (parser.is_set('a')) aaa = std::stoi(parser.get_argument('a'));

    fm_index.backward_search(queries, hits, parser.is_set('b'), aaa);
    std::chrono::system_clock::time_point end = std::chrono::system_clock::now();

    std::chrono::duration<double> elapsed_seconds = end - start;
    std::time_t end_time = std::chrono::system_clock::to_time_t(end);

    if (hits.size() == 1) {
        std::cout << "[SEARCH] " << hits.size() << " query is found in " <<
                  elapsed_seconds.count() << " seconds.\n";
    } else {
        std::cout << "[SEARCH] " << hits.size() << " queries are found in " <<
                  elapsed_seconds.count() << " seconds.\n";
    }

    std::ofstream fout{output_file};

    if (!fout.is_open()) {
        std::cerr << "[SEARCH] Can't write file: " << output_file << "\n";
        return 1;
    }

    size_t percent = (hits.size()/queries.size())*100;

    fout << "Hit report:    Search " << query_file << " in " << index_file << ".\n";
    fout << "               Found " << hits.size() << " queries (" << percent << "%) in " <<
                                elapsed_seconds.count() << " seconds, finished at" << std::ctime(&end_time) << "\n";
    fout << "Hits:\n";

    for (const struct hit &hit: hits) {
        fout << "<" << hit.query;

        for (const std::pair<uint32_t, uint16_t> position : hit.hit_vec) {
            fout << " (" << position.first << "," << position.second << ")";
        }

        fout << ">\n";
    }

    fout.close();

    return 0;
}