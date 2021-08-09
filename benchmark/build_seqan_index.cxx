#include <string>
#include <vector>
#include <fstream>

#include <chrono>

#include <cereal/archives/binary.hpp>
#include <seqan3/argument_parser/argument_parser.hpp>
#include <seqan3/core/debug_stream.hpp>
#include <seqan3/io/sequence_file/input.hpp>
#include <seqan3/alphabet/aminoacid/aa27.hpp>
#include <seqan3/search/fm_index/fm_index.hpp>

int main (int argc, char * argv[])
{
    std::string input_file{};
    std::string output_file{};

    seqan3::argument_parser parser("Build_Index", argc, argv);
    parser.add_positional_option(input_file, "The name and the path of the file to read.");
    parser.add_positional_option(output_file, "The name and the path of the file to write");

    try {
        parser.parse();
    } catch (seqan3::argument_parser_error const & ext) {
        seqan3::debug_stream << "[PARSER ERROR] " << ext.what() << "\n";
        return -1;
    }

    seqan3::debug_stream << "[BUILD SEQAN INDEX] Reading file " << input_file << "\n";

    std::vector<seqan3::aa27_vector> sequences{};

    seqan3::sequence_file_input<seqan3::sequence_file_input_default_traits_aa> file_in{input_file};

    for (auto & [seq, id , qual] : file_in) {
        sequences.push_back(seq);
    }

    std::cout << "[BUILD SEQAN INDEX] Build index\n";


    auto start = std::chrono::steady_clock::now();

    seqan3::fm_index index{sequences};

    auto end = std::chrono::steady_clock::now();
    std::chrono::duration<double> elapsed_seconds = end - start;

    std::cout << "[BUILD SEQAN INDEX] Index built up in " << elapsed_seconds.count() << " seconds.\n";

    std::cout << "[BUILD SEQAN INDEX] Write output file\n";

    std::ofstream fout{output_file, std::ios::binary};

    // CHECK: if output stream is open
    if (!fout.is_open()) {
        std::cerr << "[WRITE INDEX] Can't write file: " << output_file << "\n";
        return -1;
    }

    cereal::BinaryOutputArchive oarchive{fout};
    oarchive(index);

    fout.close();

    return 0;
}
