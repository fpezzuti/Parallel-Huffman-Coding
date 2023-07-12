#include "./../include/utils.hpp"


bool Comparator::operator()(const HuffNode* n1,const HuffNode* n2) const {
        return *n1>*n2;
}

void countSymbolFrequencies(const std::string& fileString, int start, int size, std::unordered_map<char, int>& freqs){

    for(int i=0; i<size; ++i)
        freqs[fileString[start+i]] += 1;
}


bool readFile(const std::string& input_file, std::string* file_string, const int fsize){

    bool success = true;

    ifstream in(INPUT_DATA_PATH+input_file, std::ios::in); // open file input stream

    if(in.is_open()) {
        std::vector<char> buffer(fsize+1, 0);
        
        in.read(buffer.data(), fsize); // read string
        *file_string += buffer.data();
    }

    if(!in.eof() && in.fail()){
        std::cout << "error reading " + input_file +"\n";
        success = false;
        exit(0);
    }

    in.close();

    return success;
}


void printSymbolFrequencies(const std::vector<HuffNode*>& symbolFreqs){
    std::cout << "\nALPHABET:\n";
    for(auto it = std::begin(symbolFreqs); it != std::end(symbolFreqs); ++it)
        std::cout << "\t(" << (*it)->symbol << "," << (*it)->freq << ")\n";

}

HuffNode* buildHuffTree(std::unordered_map<char, int>* alphabet){

    std::vector<HuffNode*> leaves;

    for(auto const& symb : *alphabet){
        leaves.push_back(new HuffNode(string(1, symb.first), symb.second));
    }

    //printSymbolFrequencies(leaves);
    
    std::make_heap(leaves.begin(), leaves.end(), Comparator()); // create minHeap

    while(leaves.size() >1){
        HuffNode* n1;
        HuffNode* n2;
        
        n1 = new HuffNode(*leaves[0]); // get minHeap's root


        std::pop_heap(leaves.begin(), leaves.end(), Comparator()); // simulate minHeap's root deletion 
        delete leaves.back();
        leaves.pop_back(); // remove the root (last element)


        n2 = new HuffNode(*leaves[0]); // get minHeap's new root
        std::pop_heap(leaves.begin(), leaves.end(), Comparator()); // simulate minHeap's new root deletion
        delete leaves.back();
        leaves.pop_back(); // remove the root (last element)


        leaves.push_back(new HuffNode(n1->symbol+n2->symbol, n1->freq+n2->freq, n1, n2)); 

        std::push_heap(leaves.begin(), leaves.end(), Comparator());   

    }

    return leaves.front();
}

void deleteTree(HuffNode*& tree){
    
    if(!tree || tree == NULL)
        return;

    tree->destroyNode();

    if(tree->left_child==NULL && tree->right_child== NULL){
        delete tree;
        tree = NULL;
        return;
    }
};

std::string& compressSymbol(const char& c, const HuffNode& tree, std::string& bit_string){

    const HuffNode* ptr_symbol = &tree;
    while((ptr_symbol->symbol).compare(string(1,c))!=0){
        
        if(ptr_symbol->left_child!=NULL && (ptr_symbol->left_child)->symbol.find(c) != std::string::npos){
            bit_string.push_back('0');
            ptr_symbol = ptr_symbol->left_child;
            
        } else if(ptr_symbol->right_child!=NULL && (ptr_symbol->right_child)->symbol.find(c) != std::string::npos){
            bit_string.push_back('1');
            ptr_symbol = ptr_symbol->right_child;
        }
    }

    return bit_string;
}

void printCompressionTable(const std::unordered_map<char, std::string>& compression_table){

    std::cout << "\nHUFFMAN COMPRESSION TABLE:\n";
    std::cout << "\t Symbol: \t\t Code:\n";

    for(auto it = compression_table.cbegin(); it != compression_table.cend(); ++it)
        std::cout << "\t\t " << it->first << " \t\t\t " << it->second << "\n";

    std::cout << "\n";
}

std::unordered_map<char, std::string> buildCompressionTable(const HuffNode& tree){
    
    std::unordered_map<char, std::string> compression_table;

    for(const char& c : tree.symbol)
        compression_table[c] = compressSymbol(c, tree, compression_table[c]);

    return compression_table;
}

int convertToWritableBitString(const std::string& bit_string, std::vector<unsigned char>& cfile, int start){
    unsigned char compressed_byte = 0;

    int i=0;

    int c = 7-start;

    if(start != 0){ // check if there are remaining bits to be written
        compressed_byte = cfile[0];
        cfile.erase(cfile.begin());
    }

    while(bit_string[i] != '\0') {
        compressed_byte |= ((bit_string[i] - '0') << c);

        // decrement modulo 7
        c -= 1;

        // check if the byte is full
        if(c < 0) {
            //std::cout << "writing to file: " << std::bitset<8>(compressed_byte) << std::endl;
            // write the byte to file
            cfile.push_back(compressed_byte);
            // reset the byte
            compressed_byte ^= compressed_byte;

            c = 7;
        }
        ++i;
    }

    // write the remaining bits to file (if any)
    if(c!=7){
        //std::cout << "remaining to write: " << std::bitset<8>(compressed_byte) << "(" << 7-c << " bits)\n";
        cfile.push_back(compressed_byte);
    }

    return 7-c; // number of written bits in the last byte
}


bool writeConvertedBitStringToFile(std::vector<unsigned char>& cfile, const std::string& output_file, int wbits){

    int wsize = cfile.size() - (wbits > 0);

    std::ofstream filestream(OUTPUT_DATA_PATH+output_file+".bin", std::ios::app | std::ios::binary);

    filestream.write((char *)&cfile[0], wsize); 

    if(filestream.fail()) {
        std::cout << "Failed writing the compressed file: " << output_file  << "\n";
        exit(0);
        return false;
    }

    filestream.close();
    
    cfile.erase(cfile.begin(), cfile.begin()+wsize);

    return true;
}
