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

Network::Network(size_t vertex_quantity):Graph(vertex_quantity),max_flow_(0){}
Network::Network():Graph(),max_flow_(0){}
Network::Network(const Graph & graph):
Graph(graph),
max_flow_(0) {
    is_back_edge_.resize(edges_.size() * 2);
    flow_.resize(edges_.size());
    back_edge_ind_.resize(edges_.size() * 2);
    size_t i;
    for (i = 0; i < edges_.size(); ++i) {
        flow_[i] = 0;
        is_back_edge_[i] = false;
    }
    
    edges_.reserve(edges_.size() * 2);
    for(size_t j = 0; j < flow_.size(); ++j) {
        size_t from = edges_[j].to;
        size_t to = edges_[j].from;
        size_t capacity = edges_[j].capacity;
        edges_.push_back(Edge(from,to,capacity));
        is_back_edge_[i + j] = true;
        back_edge_ind_[i + j] = j;
        back_edge_ind_[j] = i;
     }
}

inline const std::vector<size_t> & Network::getOutgoingEdges(size_t v_ind) const {
    return vertex_[v_ind].outgoing;
}

inline const Edge & Network::getEdge(size_t e_ind) const {
    return edges_[e_ind];
}

inline size_t Network::getNetworkSizeV() const {
    return vertex_.size();
}

inline size_t Network::backEdge(size_t ind) const {
    return back_edge_ind_[ind];
}

inline ui64 Network::getEdgeFlow(size_t ind) const {
    if(!is_back_edge_[ind]) return flow_[ind];
    ind = backEdge(ind);
    return edges_[ind].capacity - flow_[ind];
}

size_t Network::createNewEdgeFromNetwork(size_t e_ind, const Network & net) {
    const Edge parent_edge = net.getEdge(e_ind);
    Edge newEdge(parent_edge.from,parent_edge.to,net.getAllowedCapacity(e_ind));
    
    size_t ind = addNewEdge(newEdge);
    is_back_edge_.push_back(net.is_back_edge_[e_ind])
}
inline unsigned long long Network::pushFlow(size_t ind, ui64 value) {
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
    return excess;
}

void Network::setFlow(size_t ind, unsigned long long value) {
    pushFlow(backEdge(ind), getEdgeFlow(ind)); // set 0
    pushFlow(ind, value);
}

inline unsigned long long Network::getAllowedCapacity(size_t ind) const {
    if(!is_back_edge_[ind]) return edges_[ind].capacity - flow_[ind];
    return flow_[backEdge(ind)];
}

unsigned long long Network::countCurrentFlow(size_t s) const {
    ui64 cur_flow = 0;
    const Vertex & cur = vertex_[s];
    for(ui64 i = 0; i < cur.outgoing.size(); ++i) {
        size_t e_ind = cur.outgoing[i];
        if(!is_back_edge_[e_ind]) {
            cur_flow += getEdgeFlow(e_ind);
        }
    }
    return cur_flow;
}

void Network::bfs_(size_t start, vector<ui64> & dist) const {
    dist.resize(vertex_.size(),INF);
    
    queue<size_t> bfs_queue;
    
    dist[start] = 0;
    bfs_queue.push(start);
    
    while(!bfs_queue.empty()) {
        Vertex cur_ver = vertex_[bfs_queue.front()];
        bfs_queue.pop();
        
        vector<size_t>::iterator edge_it;
        cur_ver.outgoing.begin();
        
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