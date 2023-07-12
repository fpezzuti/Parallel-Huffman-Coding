
#ifndef SEQUENTIALHUFFMAN_H // include guards
#define SEQUENTIALHUFFMAN_H


#include <string>
#include "huffman.hpp"
#include "huff_node.hpp"
#include "utils.hpp"
#include "utimer.hpp"

class SequentialHuffuman : public HuffmanCoding {

    public:
        explicit SequentialHuffuman(const std::string input_file = "toy-test.txt");
        void Run(int nw = 1) override;
        void MeasureStats(int iter, int nw = 1) override;
};


#endif