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
#include <array>
class SuffixTree {
public:
    static const size_t INF = (size_t)(-1);
    static const size_t ALP_SIZE = 28; // small to optimize time
    class Node {
    public:
        std::array<size_t, ALP_SIZE> children;
        size_t suff_link;

        size_t p_ind; // index of parent in tree::nodes
        size_t s_pos; // the beginning of the edge position in source string

        bool is_leaf;
        size_t p_edge_len; // if node is not a leaf
    };
    
    std::vector<Node> nodes;
    SuffixTree(const std::string & source_str); // build tree
    void buildTree();
    size_t getEdgeLength(size_t ind) const;

    size_t end_text_ind;
private:
    bool is_tree_built_already_;
    
    class SuffixTreeBuilder{
    public:
        SuffixTreeBuilder(SuffixTree * tree, const std::string & source_str);
        void build();
    private:
        SuffixTree * tree_;
        const std::string str_; // source str
        
        size_t first_empty_; // first empty action position
        size_t need_link_from_; // need update suff link
        size_t edge_offset_;
        
        void addNextSymbol_(char symbol);
        void createBaseForTree_(); // create root and dummy
        size_t testFromNode_(size_t ind, char next_symbol); // use when offset == 0, create leaf or empty
        size_t testFromEdge_(size_t ind, char next_symbol); // create node + leaf or empty
        size_t makeNode_(size_t child_ind,size_t offset,char next_char); // no leaf
        size_t makeLeaf_(size_t parent, char symbol);
        char getSymbolFromEdge_(size_t ind, size_t offset) const;
        size_t findRelativeOffset_(size_t p_ind, size_t start); // find edge that need to test
    } builder_;
};
#endif /* SuffixTree_hpp */
