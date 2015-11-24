//
//  SuffixTree.cpp
//  SuffTreeUkkonen
//
//  Created by Alex on 22.11.15.
//  Copyright © 2015 Alex. All rights reserved.
//
#include "SuffixTree.hpp"

#include <vector>
#include <string>
#include <cassert>
#include <stdlib.h>

using namespace std;

SuffixTree::SuffixTree(const std::string & source_str):
end_text_ind((int)(-1)) {
    SuffixTreeBuilder(this,source_str).buildTree();
}

inline int SuffixTree::getEdgeLength(int ind) {
    Node & v = nodes[ind];
    if(v.is_leaf) {
        return end_text_ind - v.s_pos + 1;
    } else {
        return v.p_edge_len;
    }
}

/**************************SuffixTree builder**********************************/

SuffixTree::SuffixTreeBuilder::SuffixTreeBuilder(SuffixTree * source_tree, const std::string & source_str):
tree(source_tree),
str(source_str),
first_empty(1),
last_node(1),
edge_offset(0)
{}

/*create dummy node and root of tree*/
void SuffixTree::SuffixTreeBuilder::createBaseForTree() {
    tree->nodes.resize(2);
    
    // create fake node
    tree->nodes[0].is_leaf = false;
    tree->nodes[0].p_ind = 0;
    for(int i = 0; i < SuffixTree::ALP_SIZE; ++i) {
        tree->nodes[0].children[i] = 1;
    }
    
    //create root of tree
    tree->nodes[1].is_leaf = false;
    tree->nodes[1].p_ind = 0;
    tree->nodes[1].s_pos = 0;
    tree->nodes[1].p_edge_len = 1;
    memset(tree->nodes[1].children,-1,SuffixTree::ALP_SIZE * sizeof(int));
}

char SuffixTree::SuffixTreeBuilder::getChar(int ind, int offset) {
    assert(offset <= tree->getEdgeLength(ind) && offset > 0);
    Node & v = tree->nodes[ind];
    
    return str[v.s_pos + offset];
}

int SuffixTree::SuffixTreeBuilder::findRelativeOffset(int p_ind, int start) {
    while(true){
        if(edge_offset == 0) break;
        int child_ind = tree->nodes[p_ind].children[str[start]];
        int lenght = tree->getEdgeLength(child_ind);
        if(edge_offset < lenght) {
            p_ind = child_ind;
            break;
        } else {
            edge_offset -= lenght;
            start += lenght;
            p_ind = child_ind;
        }
    }
    return p_ind;
}

SuffixTree::Node & SuffixTree::SuffixTreeBuilder::makeLeaf(int parent_ind, char symb) {
    int new_size = (int)tree->nodes.size() + 1;
    tree->nodes.resize(new_size);
    Node & leaf = tree->nodes[new_size - 1];
    
    leaf.is_leaf = true;
    leaf.p_ind = parent_ind;
    
    leaf.s_pos = tree->end_text_ind;
    
    tree->nodes[parent_ind].children[symb] = new_size - 1;
    
    return leaf;
}

SuffixTree::Node & SuffixTree::SuffixTreeBuilder::makeNode(int last_ind,int offset,char next_char) {
    
    char last_char = getChar(last_ind, offset);
    tree->nodes.resize(tree->nodes.size() + 1);
    
    int new_v_ind = (int)tree->nodes.size() - 1;
    Node & new_v = tree->nodes[new_v_ind];
    Node & last_v = tree->nodes[last_ind];
    Node & parent_v = tree->nodes[last_v.p_ind];
    
    new_v.is_leaf = false;
    new_v.s_pos = last_v.s_pos;
    new_v.p_ind = last_v.p_ind;
    new_v.p_edge_len = offset;
    memset(new_v.children,-1,SuffixTree::ALP_SIZE * sizeof(int));
    
    new_v.children[last_char] = last_ind;
    
    parent_v.children[str[new_v.s_pos]] = new_v_ind;
    
    last_v.p_ind = new_v_ind;
    last_v.p_edge_len = last_v.p_edge_len - offset;
    last_v.s_pos = new_v.s_pos + offset;
    
    makeLeaf(new_v_ind, next_char);
    return tree->nodes[new_v_ind];
}

void SuffixTree::SuffixTreeBuilder::addNextSymb(char next_char) {
    ++tree->end_text_ind;
    int ind = first_empty;
    while(true) {
        int start;
        int to_link;
        int next_v_ind;
        if(edge_offset == 0) {
            to_link = ind;
            if(last_node != 1) {
                tree->nodes[last_node].suff_link = to_link;
            }
            last_node = 1;
            if(tree->nodes[ind].children[next_char] == -1) {
                Node & leaf = makeLeaf(ind, next_char);
                next_v_ind = tree->nodes[ind].suff_link;
                start = leaf.s_pos;
                to_link = leaf.p_ind;
            } else {
                ++edge_offset;
                first_empty = tree->nodes[ind].children[next_char];
                if(tree->getEdgeLength(first_empty) == 1) {
                    edge_offset = 0;
                }
                break;
            }
        }
        else {
            char expected_char = getChar(ind, edge_offset);
            if(expected_char == next_char) {
                if(edge_offset + 1 == tree->getEdgeLength(ind)) {
                    edge_offset = 0;
                    to_link = ind;
                    if(last_node != 1) {
                        tree->nodes[last_node].suff_link = to_link;
                    }
                    last_node = 1;
                }
                else {
                    ++edge_offset;
                }
                first_empty = ind;
                break;
            }
            else {
                Node & new_v = makeNode(ind, edge_offset, next_char);
                next_v_ind = tree->nodes[new_v.p_ind].suff_link;
                start = new_v.s_pos;
                to_link = tree->nodes[new_v.children[next_char]].p_ind;
            }
        }
        
        if(last_node != 1) {
            tree->nodes[last_node].suff_link = to_link;
        }
        last_node = to_link;
        
        ind = findRelativeOffset(next_v_ind,start);
    }
}

void SuffixTree::SuffixTreeBuilder::buildTree() {
    createBaseForTree();
    for(int i = 0; i < str.size(); ++i) {
        addNextSymb(str[i]);
    }
}
