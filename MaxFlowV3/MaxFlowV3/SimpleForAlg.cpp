//
//  SimpleForAlg.cpp
//  MaxFlowV3
//
//  Created by Alex on 12.10.15.
//  Copyright © 2015 Alex. All rights reserved.
//

#include "MainHeader.h"
using std::vector;
typedef unsigned long long ui64;

SimpleForAlg::SimpleForAlg(Graph graph):
network_(graph),
cur_edge_to_discharge_(network_.getNetworkSizeV(),0){}

ui64 SimpleForAlg::getValueOfMaxFlow() const {
    return network_.getMaxFlow();
}

const Network & SimpleForAlg::returnNetwork() const {
    return network_;
}

void SimpleForAlg::setValueOfMaxFlow_() {
    network_.setMaxFlow(network_.countCurrentFlow(start_));
}

void SimpleForAlg::findMaxFlow(size_t st, size_t fin)
{
    start_ = st;
    finish_ = fin;

    height_.resize(network_.getNetworkSizeV());
    excess_.resize(network_.getNetworkSizeV());
    
    for (size_t i = 0; i < network_.getNetworkSizeV(); ++i)
    {
        height_[i] = 0;
        excess_[i] = 0;
    }
    height_[start_] = network_.getNetworkSizeV();
    
    //start init
    const vector<size_t> & outgoing = network_.getOutgoingEdges(start_);
    for(size_t i = 0 ; i < outgoing.size(); ++i)
    {
        size_t e_ind = outgoing[i];
        const Edge & cur_edge = network_.getEdge(e_ind);
        excess_[cur_edge.to] += cur_edge.capacity;
        network_.setEdgeFlow(e_ind, cur_edge.capacity);
    }
    excess_[finish_] = 0;
    excess_[start_] = 0;

    bool is_updated = true;
    while (is_updated) {
        is_updated = false;
        for (int v_ind = 0; v_ind < network_.getNetworkSizeV(); ++v_ind) {
            if(excess_[v_ind] > 0) {
                is_updated = true;
                discharge_(v_ind);
            }
        }
    }
    setValueOfMaxFlow_();
}

void SimpleForAlg::discharge_(size_t v_ind)
{
    size_t e_ind = cur_edge_to_discharge_[v_ind]; // to continue with last point
    const vector<size_t> & outgoing = network_.getOutgoingEdges(v_ind);
    while(excess_[v_ind] > 0) {
        if(e_ind + 1 == outgoing.size()){
            relabel_(v_ind);
            e_ind = 0;
            continue;
        }
        
        const Edge & edge = network_.getEdge(e_ind);
        if(height_[edge.from] == height_[edge.to] + 1 && network_.getAllowedCapacity(e_ind) > 0) {
            push_(e_ind);
        }
        ++e_ind;
    }
    cur_edge_to_discharge_[v_ind] = e_ind;
}

void SimpleForAlg::push_(size_t e_ind) {
    const Edge & cur_edge = network_.getEdge(e_ind);
    
    ui64 pushed = network_.pushFlow(e_ind, excess_[cur_edge.from]);
    excess_[cur_edge.to] += pushed;
    excess_[cur_edge.from] -= pushed;
    
    if(cur_edge.to == finish_ || cur_edge.to == start_) excess_[cur_edge.to] = 0;
}

void SimpleForAlg::relabel_(size_t v_ind) {
    ui64 min_h = 2 * network_.getNetworkSizeV() + 3;
    const vector<size_t> & outgoing = network_.getOutgoingEdges(v_ind);
    for(size_t i = 0; i < outgoing.size(); ++i) {
        size_t e_ind = outgoing[i];
        const Edge & cur_e = network_.getEdge(e_ind);
        
        if(network_.getAllowedCapacity(e_ind) > 0){
            if(height_[cur_e.to] < min_h) min_h = height_[cur_e.to];
        }
    }
    height_[v_ind] = min_h + 1;
}
