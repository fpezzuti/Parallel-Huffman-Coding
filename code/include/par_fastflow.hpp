#ifndef FASTFLOWHUFFMAN_H // include guards
#define FASTFLOWHUFFMAN_H

#include <ff/ff.hpp>
#include <ff/parallel_for.hpp>
#include <string>
#include <vector>
#include <functional>
#include <filesystem>
#include <stdio.h>
#include "huffman.hpp"
#include "huff_node.hpp"
#include "utils.hpp"

using namespace ff;

extern std::string fstring_ff;
extern std::unordered_map<char, std::string> compression_table_ff;

class Task {
    public:
        int start, csize;
        std::string compressed_bstring;
           
        Task(int from, int size): start(from), csize(size){ };
};

struct Worker_compression: ff_node_t<Task> {
    Worker_compression(){};
    Task* svc(Task* t);
};

struct Emitter_compression: ff_node_t<Task> {
    const int nw;

    Emitter_compression(const int nw): nw(nw) {};

    Task* svc(Task*);

};

struct Collector_compression: ff_node_t<Task> {

    const std::string fname;

    std::vector<unsigned char> writable_bitstring;

    int to_collect, wbits = 0;

    Collector_compression(const std::string& fname, int nw): fname(fname), to_collect(nw) {};

    Task* svc(Task* t);
};

bool PipeMapCompression(const std::string& fname, const int nw);

class ParFastflowHuffuman : public HuffmanCoding {

    public:
        explicit ParFastflowHuffuman(const std::string& input_file = "toy-test.txt");
        void Run(int nw = 1) override;
        void MeasureStats(int iter, int nw) override;
};

#endif