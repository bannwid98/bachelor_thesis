#include "../fm_index/fm_index.h"
#include "../cmd_parser/cmd_parser.h"

int main(int argc, char** argv) {

    std::vector<std::pair<char, std::pair<bool, std::string>>> options{
            {'i', {false, ""}},
            {'o', {false, ""}},
            {'r', {false, ""}}
    };

    CMD_Parser parser{options};

    parser.parse(argc, argv);

    std::string input_file{parser.get_argument('i')};
    std::string output_file{parser.get_argument('o')};

    FM_Index fm_index{};

    if (!fm_index.load_index(index_file)) {
        std::cerr << "[COMPRESS SA] Can't load FM-Index\n";
        return -1;
    }

    if (fm_index.write_index(output_file, false)) {
        std::cout << "[COMPRESS SA] Write index with uncompressed BWT\n";
        return 0;
    } else {
        std::cerr << "[COMPRESS SA] Can't write index with uncompressed BWT\n";
        return -1;
    }

    std::string r{};

    if (parser.get_argument('r', r)){

        size_t rate = std::stoi(r);

        fm_index.recompress_sa(rate);
    } else {
        std::cerr << "[COMPRESS SA] Missing Argument -r\n";
        return -1;
    }

}