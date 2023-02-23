cmake_minimum_required(VERSION 3.0)
project(Wasm3-Embench LANGUAGES C)
add_compile_options(-mcpu=arm1176jzf-s -mfpu=vfpv2 -O2 -ffunction-sections)
add_link_options(-lc -lrdimon --specs=rdimon.specs -mfpu=vfpv2 -mcpu=arm1176jzf-s -O2 -flto -Wl,--gc-sections)

add_subdirectory(${CMAKE_CURRENT_LIST_DIR}/../../../../wasm3/source ${CMAKE_BINARY_DIR}/m3)
add_executable(Kernel main.c)
target_link_libraries(Kernel PRIVATE m3 PUBLIC m)
target_include_directories(Kernel BEFORE PRIVATE ../../../../embench-wasm)

set_target_properties(Kernel PROPERTIES SUFFIX ".elf")