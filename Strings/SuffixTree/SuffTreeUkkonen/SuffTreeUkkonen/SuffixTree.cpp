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
#include <iostream>

using namespace std;
typedef long long ll;

SuffixTree::SuffixTree(const std::string & source_str) {
    SuffixTreeBuilder(this,source_str).buildTree();
}

/**************************SuffixTree builder**********************************/

SuffixTree::SuffixTreeBuilder::SuffixTreeBuilder(SuffixTree * source_tree, const std::string & source_str):
str(source_str + spec_symb),
tree(source_tree),
end_text_ind((size_t)(-1)),
edge_offset(0),
first_empty(1),
last_node(1)
{}

/*create dummy node and root of tree*/
void SuffixTree::SuffixTreeBuilder::createBaseForTree() {
    tree->nodes.resize(2);

    // create fake node
    tree->nodes[0].is_leaf = false;
    tree->nodes[0].p_ind = 0;
    tree->nodes[0].children.resize(tree->ALP_SIZE);
    for(ll i = 0; i < tree->ALP_SIZE; ++i) {
        tree->nodes[0].children[i] = 1;
    }

    //create root of tree
    tree->nodes[1].children.resize(tree->ALP_SIZE,-1);
    tree->nodes[1].is_leaf = false;
    tree->nodes[1].p_ind = 0;
    tree->nodes[1].s_pos = 0;
    tree->nodes[1].p_edge_len = 1;
}

long long SuffixTree::SuffixTreeBuilder::getEdgeLength(size_t ind) {
    Node & v = tree->nodes[ind];
    if(v.is_leaf) {
        return end_text_ind - v.s_pos + 1;
    } else {
        return v.p_edge_len;
    }
}

char SuffixTree::SuffixTreeBuilder::getChar(size_t ind, long long offset) {
    assert(offset <= getEdgeLength(ind) && offset > 0);
    
    Node & v = tree->nodes[ind];
    return str[v.s_pos + offset - 1];
}

size_t SuffixTree::SuffixTreeBuilder::findRelativeOffset(size_t p_ind, size_t start) {
    while(true){
        if(edge_offset == 0) break;
        size_t child_ind = tree->nodes[p_ind].children[str[start]];
        long long lenght = tree->nodes[child_ind].p_edge_len;
        if(edge_offset < lenght) {
            p_ind = child_ind;
            break;
        } else {
            edge_offset -= lenght;
            p_ind = child_ind;
        }
    }
    return p_ind;
}

SuffixTree::Node & SuffixTree::SuffixTreeBuilder::makeLeaf(size_t parent_ind, char symb) {
    size_t new_size = tree->nodes.size() + 1;
    tree->nodes.resize(new_size);
    Node & leaf = tree-> nodes[new_size - 1];

    leaf.is_leaf = true;
    leaf.p_ind = parent_ind;
    leaf.end_text_ind_ptr = & end_text_ind;

    leaf.s_pos = end_text_ind;
    leaf.children.resize(ALP_SIZE,-1);
    
    tree->nodes[parent_ind].children[symb] = new_size - 1;
    
    return leaf;
}

SuffixTree::Node & SuffixTree::SuffixTreeBuilder::makeNode(size_t last_ind,long long offset,char next_char) {
    
    char last_char = getChar(last_ind, offset);
    tree->nodes.resize(tree->nodes.size() + 1);
    makeLeaf(tree->nodes.size()-1, next_char);
    
    size_t new_v_ind = tree->nodes.size() - 2;
    Node & new_v = tree->nodes[new_v_ind];
    Node & last_v = tree->nodes[last_ind];
    Node & parent_v = tree->nodes[last_v.p_ind];
    
    new_v.is_leaf = false;
    new_v.s_pos = last_v.s_pos;
    new_v.p_ind = last_v.p_ind;
    new_v.p_edge_len = offset;
    new_v.children.resize(ALP_SIZE,-1);
    new_v.children[last_char] = last_ind;
    
    parent_v.children[new_v.s_pos] = new_v_ind;
    
    last_v.p_ind = new_v_ind;
    last_v.p_edge_len = last_v.p_edge_len - offset;
    last_v.s_pos = new_v.s_pos + offset;
    return new_v;
}

void SuffixTree::SuffixTreeBuilder::addNextSymb(char next_char) {
    ++end_text_ind;
    size_t ind = first_empty;
    while(true) {
        size_t start;
        size_t to_link;
        size_t next_v_ind;
        if(edge_offset == 0) {
            if(tree->nodes[ind].children[next_char] == -1) {
                Node & leaf = makeLeaf(ind, next_char);
                next_v_ind = tree->nodes[ind].suff_link;
                start = leaf.s_pos;
                to_link = leaf.p_ind;
            } else {
                ++edge_offset;
                first_empty = tree->nodes[ind].children[next_char];
                if(tree->nodes[first_empty].p_edge_len == 1) {
                    edge_offset = 0;
                }
                
                break;
            }
        }
        else {
            char expected_char = getChar(ind, edge_offset);
            if(expected_char == next_char) {
                if(edge_offset == tree->nodes[ind].p_edge_len + 1) {
                    edge_offset = 0;
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
    for(size_t i = 0; i < str.size(); ++i) {
        addNextSymb(str[i]);
    }
    addNextSymb(spec_symb);
}

