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
    const static int ALP_SIZE = 28; // the smallest to optimze time
    class Node {
    public:
        int children[ALP_SIZE];
        int suff_link;

        int p_ind;
        int s_pos;

        bool is_leaf;
        int p_edge_len; // if no leaf
    };
    
    std::vector<Node> nodes;
    SuffixTree(const std::string & source_str); // build tree
    int getEdgeLength(int ind);

    int end_text_ind;
private:
    class SuffixTreeBuilder{
    public:
        SuffixTreeBuilder(SuffixTree * tree, const std::string & source_str);
        void buildTree();
    private:
        SuffixTree * tree;
        const std::string str;
        
        int first_empty; // first empty action position
        int last_node; // need update suff link
        int edge_offset;
        
        void createBaseForTree(); // create root and dummy
        void addNextSymb(char symb);
        Node & makeNode(int child_ind,int offset,char next_char); // no leaf
        Node & makeLeaf(int parent, char symb);
        char getChar(int ind, int offset);
        int findRelativeOffset(int p_ind, int start); // find edge that need to test
    };
};
#endif /* SuffixTree_hpp */
