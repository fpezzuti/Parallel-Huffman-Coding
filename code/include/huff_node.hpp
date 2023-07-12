
#ifndef HUFFTREE_H // include guards
#define HUFFTREE_H

#include <iostream>
#include <string>
#include <unistd.h>

class HuffNode {
         
    private:

        void printNodeSubTree(const std::string& prefix, const HuffNode* node, bool isLeft) const;
        
    public:
        std::string symbol;
        int freq;
        HuffNode* left_child;
        HuffNode* right_child;
        
        HuffNode();
        explicit HuffNode(const std::string& s, int f=1, HuffNode* lchild = NULL, HuffNode* rchild = NULL);
        explicit HuffNode(const std::string& symbol);
        explicit HuffNode(const char& s);
        explicit HuffNode(const HuffNode& node);

        friend std::ostream& operator<< (std::ostream& os, const HuffNode& n);
        friend bool operator== (const HuffNode& n1, const HuffNode& n2);
        friend bool operator!= (const HuffNode& n1, const HuffNode& n2);
        friend bool operator< (const HuffNode& n1, const HuffNode& n2);
        friend bool operator<= (const HuffNode& n1, const HuffNode& n2);
        friend bool operator> (const HuffNode& n1, const HuffNode& n2);
        friend bool operator>= (const HuffNode& n1, const HuffNode& n2);
        
        void printSubTree() const;
        void destroyNode();

};

#endif