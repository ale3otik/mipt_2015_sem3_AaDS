//
//  Graph.cpp
//  MaxFlowV3
//
//  Created by Alex on 12.10.15.
//  Copyright © 2015 Alex. All rights reserved.
//

#include "MainHeader.h"
#include <vector>
typedef unsigned long long ui64;
using std::vector;

Edge::Edge(){}

Edge::Edge(size_t from, size_t to, ui64 capacity){
    this->from = from;
    this->to = to;
    this->capacity = capacity;
}

Graph::Graph(){}

Graph::Graph(size_t vertex_quantity):
vertex_(vertex_quantity),
edges_(){}

Graph::Graph(size_t vertex_quantity, const vector <Edge> & edges) {
    this->edges_ = edges;
    vertex_.resize(vertex_quantity);
    
    for (int i = 0; i < edges.size(); ++i) {
        vertex_[edges[i].from].outgoing.push_back(i);
    }
}

inline const Edge & Graph::getEdge(size_t pos) const {
    return edges_[pos];
}

inline ui64 Graph::getCapacity(size_t pos) const {
    return edges_[pos].capacity;
}

inline size_t Graph::addNewEdge(const Edge & new_edge) {
    edges_.push_back(new_edge);
    return edges_.size() - 1;
}
