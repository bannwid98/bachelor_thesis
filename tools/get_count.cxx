#include "../fm_index/fm_index.h"
#include "../cmd_parser/cmd_parser.h"

int main(int argc, char** argv) {

    std::vector<std::pair<char, std::pair<bool, std::string>>> options{
            {'i', {false, ""}},
            {'o', {false, ""}},
            {'p', {true, ""}}
    };

    CMD_Parser parser{options};

    parser.parse(argc, argv);

    std::string index_file{parser.get_argument('i')};
    std::string output_file{parser.get_argument('o')};

    std::cout << "[GET COUNT] Load FM-Index\n";

    FM_Index fm_index{};

    if (!fm_index.load_index(index_file, parser.is_set('c'))) {
        std::cerr << "[GET COUNT] Can't load FM-Index\n";
        return -1;
    }

    std::array<size_t , 27> count = fm_index.get_cumulated_count();
    char letter = 'A';

    if (parser.is_set('p')) {
        for (size_t i = 1; i < count.size(); ++i) {
            std::cout << letter << " occurred " << count[i] - count[i-1]<< "\n";
            ++letter;
        }
    }

    if (parser.is_set('o')) {
        std::ofstream fout{output_file};

        if (!fout.is_open()) {
            std::cerr << "[GET COUNT] Can't write file: " << output_file << "\n";
            return -1;
        }

        for (const uint32_t number : count) {
            fout << letter << " occurred " << number << "\n";
            ++letter;
        }

        fout.close();
    }

    return 0;
}
