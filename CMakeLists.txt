cmake_minimum_required(VERSION 3.13)
project(Bachelor_Thesis)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++20 -fopenmp")
#set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fopenmp")

add_subdirectory(suffix_array)
add_subdirectory(fm_index)
add_subdirectory(cmd_parser)

#add_subdirectory(google_benchmark)

list(APPEND CMAKE_PREFIX_PATH "~/Development/seqan3/build_system")

#### Tools ####
add_executable(Build_Index tools/build_index.cxx)
add_executable(Generate_Data tools/generate_data.cxx)
add_executable(Convert_Sequences tools/convert_sequences.cxx)
add_executable(Search tools/search.cxx)
add_executable(Uncompress_BWT tools/uncompress_bwt.cxx)
add_executable(Compress_BWT tools/compress_bwt.cxx)
add_executable(Recompress_Occ_Mat tools/recompress_occ_mat.cxx)
add_executable(Test tests/test.cxx)
add_executable(Get_Count tools/get_count.cxx)

target_link_libraries(Get_Count FM_Index CMD_Parser)
target_link_libraries(Test FM_Index)
target_link_libraries(Recompress_Occ_Mat FM_Index CMD_Parser)
target_link_libraries(Uncompress_BWT FM_Index CMD_Parser)
target_link_libraries(Compress_BWT FM_Index CMD_Parser)
target_link_libraries(Build_Index FM_Index CMD_Parser)
target_link_libraries(Generate_Data CMD_Parser)
target_link_libraries(Convert_Sequences CMD_Parser)
target_link_libraries(Search FM_Index CMD_Parser)

#### Benchmarks ####
find_package(seqan3 3.0 REQUIRED)
find_package(benchmark REQUIRED)

#add_executable(Benchmark benchmark/benchmark.cxx)
add_executable(BM_Tool benchmark/bm_tool.cxx)
add_executable(BM_Load benchmark/bm_load.cxx)
add_executable(BM_Compression benchmark/bm_compression.cxx)
add_executable(Build_Seqan_Index benchmark/build_seqan_index.cxx)
add_executable(Benchmark_Build benchmark/benchmark_build.cpp)
add_executable(Benchmark benchmark/benchmark.cxx)

target_link_libraries(BM_Tool CMD_Parser)
target_link_libraries(BM_Load FM_Index CMD_Parser seqan3::seqan3)
target_link_libraries(BM_Compression FM_Index CMD_Parser)
target_link_libraries(Benchmark_Build CMD_Parser FM_Index)
target_link_libraries(Benchmark FM_Index CMD_Parser seqan3::seqan3 benchmark::benchmark)
target_link_libraries(Build_Seqan_Index seqan3::seqan3)