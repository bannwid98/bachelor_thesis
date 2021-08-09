#include "../fm_index/fm_index.h"
#include "../cmd_parser/cmd_parser.h"

int main(int argc, char** argv) {

    std::vector<std::pair<char, std::pair<bool, std::string>>> options{
            {'i', {false, ""}},
            {'o', {false, ""}}
    };

    CMD_Parser parser{options};

    parser.parse(argc, argv);

    std::string index_file{parser.get_argument('i')};
    std::string output_file{parser.get_argument('o')};

    std::cout << "[COMPRESS BWT] Load index with uncompressed BWT\n";

    FM_Index fm_index{};

    if (!fm_index.load_index(index_file, false)) {
        std::cerr << "[COMPRESS BWT] Can't load FM-Index\n";
        return -1;
    }

    if (fm_index.write_index(output_file, true)) {
        std::cout << "[COMPRESS BWT] Write index with uncompressed BWT\n";
        return 0;
    } else {
        std::cerr << "[COMPRESS BWT] Can't write index with uncompressed BWT\n";
        return -1;
    }
}