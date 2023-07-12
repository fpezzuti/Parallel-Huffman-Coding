cmake_minimum_required(VERSION 3.13)  # CMake version check

project(huffman_coding VERSION 1.0
        DESCRIPTION "Huffman Coding parallelization"
        LANGUAGES CXX)               # Create project "simple_example"
        
set(CMAKE_CXX_STANDARD 17)            # Enable c++17 standard

# find packages
find_package(Threads REQUIRED)

if (NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE Release CACHE STRING "Build type" FORCE)
endif ()
message(STATUS "Build type: ${CMAKE_BUILD_TYPE}")

set(CMAKE_CXX_FLAGS_RELEASE "-O3 -ftree-vectorize -DTRACE_FASTFLOW -pthread -g -I /usr/local/include/") # vectorization info -fopt-info-vec

# Add main.cpp file of project root directory as source file
#set(SOURCE_FILES main.cpp)

# Add executable target with source files listed in SOURCE_FILES variable
#add_executable(huffman_coding ${SOURCE_FILES})


# creating targets HuffmanCoding and HuffmanVersions
add_executable(HuffmanCoding
        ${PROJECT_SOURCE_DIR}/main.cpp
        ${PROJECT_SOURCE_DIR}/src/sequential.cpp
        ${PROJECT_SOURCE_DIR}/src/utils.cpp
        ${PROJECT_SOURCE_DIR}/src/huff_node.cpp
        ${PROJECT_SOURCE_DIR}/src/par_thread.cpp
        ${PROJECT_SOURCE_DIR}/src/par_fastflow.cpp
        )
add_library(HuffmanVersions
        STATIC
        ${PROJECT_SOURCE_DIR}/include/huffman.hpp
        ${PROJECT_SOURCE_DIR}/include/utimer.hpp
        ${PROJECT_SOURCE_DIR}/include/utils.hpp
        ${PROJECT_SOURCE_DIR}/include/huff_node.hpp
        ${PROJECT_SOURCE_DIR}/include/sequential.hpp
        ${PROJECT_SOURCE_DIR}/include/par_thread.hpp
        ${PROJECT_SOURCE_DIR}/include/par_fastflow.hpp
        )

set_property(TARGET HuffmanCoding PROPERTY CXX_STANDARD 17)
set_target_properties(HuffmanVersions PROPERTIES
        CXX_STANDARD 17
        LINKER_LANGUAGE CXX)

# link libraries to main target
target_link_libraries(HuffmanCoding
        HuffmanVersions
        Threads::Threads
        )
