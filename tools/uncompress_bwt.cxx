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

    FM_Index fm_index{};

    std::cout << "[UNCOMPRESS BWT] Load index with compressed BWT\n";

    if (!fm_index.load_index(index_file, true)) {
        std::cerr << "[UNCOMPRESS BWT] Can't load FM-Index\n";
        return -1;
    }

    if (fm_index.write_index(output_file, false)) {
        std::cout << "[UNCOMPRESS BWT] Write index with uncompressed BWT\n";
        return 0;
    } else {
        std::cerr << "[UNCOMPRESS BWT] Can't write index with uncompressed BWT\n";
        return -1;
    }
}