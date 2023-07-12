#include "./../include/huff_node.hpp"

HuffNode::HuffNode(){
    symbol = '\0';
    freq = 0;
    left_child = NULL;
    right_child = NULL;
}

HuffNode::HuffNode(const std::string& s, int f, HuffNode* lchild, HuffNode* rchild){
    symbol.assign(s);
    freq = f;
    left_child = lchild;
    right_child = rchild;
}

HuffNode::HuffNode(const std::string& s){
    symbol.assign(s);
    freq = 1;
    left_child = NULL;
    right_child = NULL;
}

HuffNode::HuffNode(const char& s){
    symbol.push_back(s);
    freq = 1;
    left_child = NULL;
    right_child = NULL;
}

HuffNode::HuffNode(const HuffNode& node){
    symbol.assign(node.symbol);
    freq = node.freq;
    left_child = node.left_child;
    right_child = node.right_child;
}


void HuffNode::destroyNode(){
     if(!this || this == NULL || this==0)
        return;

    if(left_child!=0 && left_child!=NULL){
        left_child->destroyNode();
        delete left_child;
        left_child = NULL;
    }

    if(right_child!=0 && right_child!=NULL){
        right_child->destroyNode();
        delete right_child;
        right_child = NULL;
    }
}

std::ostream& operator<<(std::ostream& os, const HuffNode& n){
    os << "(" << n.symbol << ',' << n.freq << ')';
    return os;
}

bool operator==(const HuffNode& n1, const HuffNode& n2){
        return (n1.symbol == n2.symbol && n1.freq == n2.freq) ? true : false;
}

bool operator!=(const HuffNode& n1, const HuffNode& n2){
        return (n1.symbol == n2.symbol && n1.freq == n2.freq) ? false : true;
}

bool operator>(const HuffNode& n1, const HuffNode& n2){
        return n1.freq > n2.freq;
}

bool operator>=(const HuffNode& n1, const HuffNode& n2){
        return n1.freq >= n2.freq;
}

bool operator<=(const HuffNode& n1, const HuffNode& n2){
        return n1.freq <= n2.freq;
}

bool operator<(const HuffNode& n1, const HuffNode& n2){
    return n1.freq < n2.freq;
}

void HuffNode::printNodeSubTree(const std::string& prefix, const HuffNode* node, bool isLeft) const {
    if(node != NULL && node != 0){
        std::cout << prefix;

        std::cout << (isLeft ? "├──" : "└──");

        std::cout << *node << "\n";

        if(node->left_child && node->left_child!=NULL)
            printNodeSubTree( prefix + (isLeft ? "│   " : "    "), node->left_child, true);

        if(node->left_child && node->left_child!=NULL)
            printNodeSubTree( prefix + (isLeft ? "│   " : "    "), node->right_child, false);
    }
}

void HuffNode::printSubTree() const {
    std::cout << "\nHUFFMAN TREE:\n";
    printNodeSubTree("", this, false); 
}
