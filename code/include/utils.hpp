#pragma once

#include <string>
#include <iostream>
#include <vector>
#include <fstream>
#include <algorithm>
#include <unistd.h>
#include <bitset>
#include <bits/stdc++.h>
#include <map>
#include "./huff_node.hpp"

using namespace std;

const string INPUT_DATA_PATH = "./../data/inputs/";
const string OUTPUT_DATA_PATH = "./../data/outputs/";

const int ASCII_SIZE = 128;

struct Comparator{
    bool operator()(const HuffNode* n1, const HuffNode* n2) const;
};


bool readFile(const std::string& input_file, std::string* file_string, const int fsize);

void countSymbolFrequencies(const std::string& fileString, int start, int size, std::unordered_map<char, int>& freqs);
void printSymbolFrequencies(const std::vector<HuffNode*>& symbolFreqs);


HuffNode* buildHuffTree(std::unordered_map<char, int>* alphabet);
void deleteTree(HuffNode*& tree);


std::unordered_map<char, std::string> buildCompressionTable(const HuffNode& tree);
void printCompressionTable(const std::unordered_map<char, std::string>& tree);
std::string& compressSymbol(const char& c, const HuffNode& tree, std::string& bit_string);

int convertToWritableBitString(const std::string& bit_string, std::vector<unsigned char>& cfile, int start = 0);

bool writeConvertedBitStringToFile(std::vector<unsigned char>& cfile, const std::string& output_file, int wbits = 0);

