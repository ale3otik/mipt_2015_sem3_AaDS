//
//  Network.cpp
//  MaxFlowV3
//
//  Created by Alex on 12.10.15.
//  Copyright © 2015 Alex. All rights reserved.
//

#include <queue>
#include "MainHeader.h"

using std::vector;
using std::queue;
typedef long long i64;
typedef unsigned long long ui64;

Network::Network(size_t vertex_quantity):Graph(vertex_quantity),
max_flow_(0){}
Network::Network():Graph(),max_flow_(0){}
Network::Network(const Graph & graph):
Graph(graph),
max_flow_(0) {
    buildBackEdgesAndFlowInfo();
}

const std::vector<size_t> & Network::getOutgoingEdges(size_t v_ind) const {
    return vertex_[v_ind].outgoing;
}
const Edge & Network::getEdge(size_t e_ind) const {
    return edges_[e_ind];
}
size_t Network::getNetworkSizeV() const {
    return vertex_.size();
}
size_t Network::getNetworkSizeE() const {
    return edges_.size();
}
size_t Network::backEdge(size_t ind) const {
    return back_edge_ind_[ind];
}
ui64 Network::getEdgeFlow(size_t ind) const {
    if(!is_back_edge_[ind]) return flow_[ind];
    ind = backEdge(ind);
    return edges_[ind].capacity - flow_[ind];
}
bool Network::isBackEdge(size_t e_ind) const {
    return is_back_edge_[e_ind];
}

void Network::buildBackEdgesAndFlowInfo() {
    edges_.reserve(edges_.size() * 2);
    
    flow_.resize(edges_.size(),0);
    is_back_edge_.resize(edges_.size() * 2);
    back_edge_ind_.resize(edges_.size() * 2);
    size_t r = edges_.size();
    
    for(size_t j = 0; j < flow_.size(); ++j) {
        size_t from = edges_[j].to;
        size_t to = edges_[j].from;
        size_t capacity = edges_[j].capacity;
        edges_.push_back(Edge(from,to,capacity));
        vertex_[from].outgoing.push_back(r + j);
        
        is_back_edge_[r + j] = true;
        is_back_edge_[j] = false;
        back_edge_ind_[r + j] = j;
        back_edge_ind_[j] = r + j;
    }
}

ui64 Network::pushFlow(size_t ind, ui64 value) {
    Edge cur;
    ui64 excess;
    if(is_back_edge_[ind]) {
        ind = backEdge(ind);
        cur = edges_[backEdge(ind)];
        excess = (value > flow_[ind]) ? (value - flow_[ind]) : 0;
        flow_[ind] -= value - excess;

    } else {
        cur = edges_[ind];
        excess = (value > cur.capacity - flow_[ind]) ? (value - (cur.capacity - flow_[ind])) : 0;
        flow_[ind] += value - excess;
    }
    return value - excess;
}

void Network::setEdgeFlow(size_t ind, ui64 value) {
    pushFlow(backEdge(ind), getEdgeFlow(ind)); // set 0
    pushFlow(ind, value);
}

ui64 Network::getAllowedCapacity(size_t ind) const {
    if(!is_back_edge_[ind]) return edges_[ind].capacity - flow_[ind];
    return flow_[backEdge(ind)];
}

ui64 Network::countCurrentFlow(size_t s) const {
    i64 cur_flow = 0;
    const Vertex & cur = vertex_[s];
    for(ui64 i = 0; i < cur.outgoing.size(); ++i) {
        size_t e_ind = cur.outgoing[i];
        if(!is_back_edge_[e_ind]) {
            cur_flow += getEdgeFlow(e_ind);
        } else {
            cur_flow -= getEdgeFlow(backEdge(e_ind));
        }
    }
    return cur_flow;
}
vector<ui64> Network::countDist(size_t start) const {
    std::vector<ui64> dist;
    bfs_(start, dist);
    return dist;
}

void Network::bfs_(size_t start, vector<ui64> & dist) const {
    dist.resize(vertex_.size(),INF);
    
    queue<size_t> bfs_queue;
    
    dist[start] = 0;
    bfs_queue.push(start);
    
    while(!bfs_queue.empty()) {
        Vertex cur_ver = vertex_[bfs_queue.front()];
        bfs_queue.pop();
        
        vector<size_t>::iterator edge_it = cur_ver.outgoing.begin();
        
        while(edge_it != cur_ver.outgoing.end()) {
            Edge cur_edge = edges_[*edge_it];
            size_t to = cur_edge.to;
            size_t from = cur_edge.from;
            if(getAllowedCapacity(*edge_it) > 0 && dist[to] == INF) {
                dist[to] = dist[from] + 1;
                bfs_queue.push(to);
            }
            ++edge_it;
        }
    }
}