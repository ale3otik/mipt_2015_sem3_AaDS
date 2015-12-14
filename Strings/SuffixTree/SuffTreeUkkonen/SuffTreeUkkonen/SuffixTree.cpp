//
//  SuffixTree.cpp
//  SuffTreeUkkonen
//
//  Created by Alex on 22.11.15.
//  Copyright © 2015 Alex. All rights reserved.
//
#include "SuffixTree.hpp"

/******************************SuffixTree*********************************/
SuffixTree::SuffixTree(const std::string & source_str):
end_text_ind(INF),
builder_(this,source_str),
is_tree_built_already_(false) {}

void SuffixTree::buildTree() {
    if(is_tree_built_already_) return;
    is_tree_built_already_ = true;
    
    builder_.build();
}

size_t SuffixTree::getEdgeLength(size_t ind) const {
    const Node & v = nodes[ind];
    if(v.is_leaf) {
        return end_text_ind - v.s_pos + 1;
    } else {
        return v.p_edge_len;
    }
}

/**********************SuffixTree builder**********************************/
const size_t SuffixTree::INF;

SuffixTree::SuffixTreeBuilder::SuffixTreeBuilder(SuffixTree * source_tree, const std::string & source_str):
tree_(source_tree),
str_(source_str),
first_empty_(1),
need_link_from_(1),
edge_offset_(0) {}

/*create dummy node and root of tree*/
void SuffixTree::SuffixTreeBuilder::createBaseForTree_() {
    tree_->nodes.reserve(2 * str_.size() + 4);
    tree_->nodes.resize(2);

    // create dummy node
    Node & dummy = tree_->nodes[0];
    dummy.is_leaf = false;
    dummy.p_ind = 0;
    dummy.children.fill(1);
    
    //create root of tree
    Node & root = tree_->nodes[1];
    root.is_leaf = false;
    root.p_ind = 0;
    root.s_pos = 0;
    root.p_edge_len = 1;
    root.children.fill(INF);
}

char SuffixTree::SuffixTreeBuilder::getSymbolFromEdge_(size_t ind, size_t offset) const {
    const Node & v = tree_->nodes[ind];
    return str_[v.s_pos + offset];
}

size_t SuffixTree::SuffixTreeBuilder::findRelativeOffset_(size_t p_ind, size_t s_position) {
    while(true){
        if(edge_offset_ == 0) break;
        size_t child_ind = tree_->nodes[p_ind].children[str_[s_position]];
        size_t lenght = tree_->getEdgeLength(child_ind);
        if(edge_offset_ < lenght) {
            p_ind = child_ind;
            break;
        } else {
            edge_offset_ -= lenght;
            s_position += lenght;
            p_ind = child_ind;
        }
    }
    return p_ind;
}

size_t SuffixTree::SuffixTreeBuilder::makeLeaf_(size_t parent_ind, char symb) {
    size_t new_size = tree_->nodes.size() + 1;
    tree_->nodes.resize(new_size);
    Node & leaf = tree_->nodes[new_size - 1];
    
    leaf.is_leaf = true;
    leaf.p_ind = parent_ind;
    leaf.s_pos = tree_->end_text_ind;
    tree_->nodes[parent_ind].children[symb] = new_size - 1;
    
    return new_size - 1;
}

size_t SuffixTree::SuffixTreeBuilder::makeNode_(size_t last_ind, size_t offset, char next_char) {
    char last_char = getSymbolFromEdge_(last_ind, offset);
    tree_->nodes.resize(tree_->nodes.size() + 1);
    
    size_t new_v_ind = tree_->nodes.size() - 1;
    Node & new_v = tree_->nodes[new_v_ind];
    Node & last_v = tree_->nodes[last_ind];
    Node & parent_v = tree_->nodes[last_v.p_ind];
    
    new_v.is_leaf = false;
    new_v.s_pos = last_v.s_pos;
    new_v.p_ind = last_v.p_ind;
    new_v.p_edge_len = offset;
    new_v.children.fill(INF);
    new_v.children[last_char] = last_ind;
  
    parent_v.children[str_[new_v.s_pos]] = new_v_ind;
    
    last_v.p_ind = new_v_ind;
    last_v.p_edge_len = last_v.p_edge_len - offset;
    last_v.s_pos = new_v.s_pos + offset;
    
    makeLeaf_(new_v_ind, next_char);
    return new_v_ind;
}

size_t SuffixTree::SuffixTreeBuilder::testFromEdge(size_t ind, char next_symbol) {
    char expected_char = getSymbolFromEdge_(ind, edge_offset_);
    if(expected_char == next_symbol) {
        if(edge_offset_ + 1 == tree_->getEdgeLength(ind)) {
            edge_offset_ = 0;
            if(need_link_from_ != 1) {
                tree_->nodes[need_link_from_].suff_link = ind;
                need_link_from_ = 1;
            }
        } else {
            ++edge_offset_;
        }
        first_empty_ = ind;
        return INF;
    }
    
    return makeNode_(ind, edge_offset_, next_symbol);
}

size_t SuffixTree::SuffixTreeBuilder::testFromNode(size_t ind, char next_symbol) {
    if(need_link_from_ != 1) {
        tree_->nodes[need_link_from_].suff_link = ind;
        need_link_from_ = 1;
    }
    
    if(tree_->nodes[ind].children[next_symbol] != INF) {
        ++edge_offset_;
        first_empty_ = tree_->nodes[ind].children[next_symbol];
        if(tree_->getEdgeLength(first_empty_) == 1) {
            edge_offset_ = 0;
        }
        return INF;
    }
    
    return makeLeaf_(ind, next_symbol);
}

void SuffixTree::SuffixTreeBuilder::addNextSymbol_(char next_symbol) {
    ++tree_->end_text_ind;
    size_t ind = first_empty_;
    while(true) {
        size_t new_v_ind;
        size_t link_to;
        if(edge_offset_ == 0) {
            new_v_ind = testFromNode(ind, next_symbol);
            if(new_v_ind == INF) break;
            link_to = tree_->nodes[new_v_ind].p_ind;
        } else {
            new_v_ind = testFromEdge(ind, next_symbol);
            if(new_v_ind == INF) break;
            link_to = new_v_ind;
        }
        
        const Node & new_v = tree_->nodes[new_v_ind];
        size_t next_v_ind = tree_->nodes[new_v.p_ind].suff_link;
        size_t str_position = new_v.s_pos;
        
        if(need_link_from_ != 1) {
            tree_->nodes[need_link_from_].suff_link = link_to;
        }
        need_link_from_ = link_to;
        ind = findRelativeOffset_(next_v_ind,str_position);
    }
}

void SuffixTree::SuffixTreeBuilder::build() {
    createBaseForTree_();
    for(size_t i = 0; i < str_.size(); ++i) {
        addNextSymbol_(str_[i]);
    }
}
