#ifndef THREADSHUFFMAN_H // include guards
#define THREADSHUFFMAN_H

#include <string>
#include <vector>
#include <functional>
#include <filesystem>
#include <stdio.h>
#include "huffman.hpp"
#include "huff_node.hpp"
#include "utils.hpp"

template <typename Tout> Tout MapAndReduceFreqs(
    int nw,
    std::string& input,
    std::function<Tout (const std::string&, int, int)> mapfun
    );

void reducePartialresults(std::vector<std::unordered_map<char, int>>& partial_maps);

template <typename Tout> bool PipeMapAndReduceCompression(
    int nw,
    const std::string& fname,
    std::string& input,
    std::function<Tout (const std::string&, int, int)> mapfun,
    std::function<bool(const std::string&, Tout)> redfun
    );

class ParThreadHuffuman : public HuffmanCoding {

    public:
        explicit ParThreadHuffuman(const std::string& input_file = "toy-test.txt");
        void Run(int nw = 1) override;
        void MeasureStats(int iter, int nw) override;
};


#endif