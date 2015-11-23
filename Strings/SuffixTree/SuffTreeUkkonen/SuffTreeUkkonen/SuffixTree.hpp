//
//  SuffixTree.hpp
//  SuffTreeUkkonen
//
//  Created by Alex on 22.11.15.
//  Copyright © 2015 Alex. All rights reserved.
//

#ifndef SuffixTree_hpp
#define SuffixTree_hpp
#include <vector>
#include <string>
class SuffixTree {
public:
    class Node {
    public:
        std::vector<long long> children;
        size_t suff_link;

        size_t p_ind;
        size_t s_pos;

        bool is_leaf;
        long long p_edge_len; // if no leaf
        size_t * end_text_ind_ptr; // right index of leaf's parent edge
    };

    std::vector<Node> nodes;
    
    const static int ALP_SIZE = 128;
    
    SuffixTree(const std::string & source_str);
    
private:
    class SuffixTreeBuilder{
    public:
        SuffixTreeBuilder(SuffixTree * tree, const std::string & source_str);
        void buildTree();
    private:
        const char spec_symb = 1;
        SuffixTree * tree;
        const std::string str;

        size_t end_text_ind;
        size_t first_empty;
        size_t last_node; // need update suff link
        long long edge_offset;
        
        void createBaseForTree();
        void addNextSymb(char symb);
        Node & makeNode(size_t child_ind,long long offset,char next_char);
        Node & makeLeaf(size_t parent, char symb);
        char getChar(size_t ind, long long offset);
        long long getEdgeLength(size_t ind);
        size_t findRelativeOffset(size_t p_ind, size_t start);
        
    };
};
#endif /* SuffixTree_hpp */
