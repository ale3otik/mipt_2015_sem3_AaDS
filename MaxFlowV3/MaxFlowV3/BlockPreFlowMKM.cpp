//
//  BlockPreFlowMKM.cpp
//  MaxFlowV3
//
//  Created by Alex on 14.10.15.
//  Copyright © 2015 Alex. All rights reserved.
//

#include <assert.h>
#include "MainHeader.h"

using std::vector;
using std::queue;
typedef unsigned long long ui64;

BlockPreflowMKM::BlockPreflowMKM(Graph graph):
network_(graph),
lvl_net_(){}
Network BlockPreflowMKM::returnNetwork() const{
    return this->network_;
}

/***************************************************/

void BlockPreflowMKM::makeLvlNet_(const vector<ui64> & dist) {
    lvl_net_ = Network(network_.getNetworkSizeV());
    out_edge_pointer_ = vector<size_t>(network_.getNetworkSizeV(),0);
    in_edge_pointer_ = vector<size_t>(network_.getNetworkSizeV(),0);
    is_deleted_vertex_ = vector<bool>(network_.getNetworkSizeV(),false);
    associated_.resize(0);
    excess_ = vector<ui64>(network_.getNetworkSizeV(),0);
    ui64 new_edges_size = 0;
    for(size_t v = 0; v < network_.getNetworkSizeV(); ++v)
    {
        const vector<size_t> & outgoing = network_.getOutgoingEdges(v);
        for(size_t e = 0; e < outgoing.size(); ++e) {
            const Edge & e_cur = network_.Graph::getEdge(outgoing[e]);
            
            if(dist[e_cur.to] == dist[v] + 1 && dist[e_cur.to] <= dist[finish_] && network_.getAllowedCapacity(e) > 0 ) {
                lvl_net_.createNewEdgeFromNetwork(e, network_);
                associated_.push_back(e);
                associated_.push_back(0); // to save numeration with back edges
                ++new_edges_size;
            }
        }
    }
    is_deleted_edge_ = vector<bool>(new_edges_size,false);
}

void BlockPreflowMKM::initPotential_() {
    potential_in_ = vector<ui64>(network_.getNetworkSizeV(),0);
    potential_out_ = vector<ui64>(network_.getNetworkSizeV(),0);
    
    for(int v = 0 ; v < lvl_net_.getNetworkSizeV(); ++v)
    {
        const vector<size_t> & outgoing = lvl_net_.getOutgoingEdges(v);
        for(int e = 0; e < outgoing.size(); ++ e)
        {
            potential_in_[v] += lvl_net_.getAllowedCapacity(e);
            potential_out_[v] += lvl_net_.getAllowedCapacity(lvl_net_.backEdge(e));
        }
    }
}

void BlockPreflowMKM::bfsDeleteEmptyNodes_(size_t s) {
    
    queue<size_t> next_deleted;
    next_deleted.push(s);
    while (!next_deleted.empty()) {
        s = next_deleted.front();
        next_deleted.pop();
        
        if(is_deleted_vertex_[s] || s == start_ || s == finish_) continue;
        
        const vector<size_t> & outgoing = lvl_net_.getOutgoingEdges(s);

        size_t i = std::min(out_edge_pointer_[s],in_edge_pointer_[s]);
        for(; i < outgoing.size(); ++ i) {

            size_t e_ind = outgoing[i];
            const Edge & e = lvl_net_.getEdge(e_ind);
        
            if(!is_deleted_edge_[e_ind]) {
                potential_in_[e.to] -= lvl_net_.getAllowedCapacity(e_ind);
                potential_out_[e.to] -= lvl_net_.getAllowedCapacity(lvl_net_.backEdge(e_ind));
            }
            is_deleted_edge_[e_ind] = true;
            is_deleted_edge_[lvl_net_.backEdge(e_ind)] = true;
        }
        is_deleted_vertex_[s] = true;
    }
}

void BlockPreflowMKM::pushFlow_(size_t s, bool push_back)
{
    queue<size_t> next_vertices;
    next_vertices.push(s);
    while(!next_vertices.empty())
    {
        s = next_vertices.front();
        next_vertices.pop();
        if((s == finish_ && !push_back) || (s == start_ && push_back)) continue;
        const vector<size_t> & outgoing = lvl_net_.getOutgoingEdges(s);

        vector<size_t> & edge_pointer = push_back ? in_edge_pointer_ : out_edge_pointer_;
    
        // to fin
        while(excess_[s] > 0 && edge_pointer[s] < outgoing.size())
        {
            if(lvl_net_.isBackEdge(outgoing[edge_pointer[s]]) ^ push_back) {
                ++ edge_pointer[s];
                continue;
            }
        
            size_t e_ind = push_back ? lvl_net_.backEdge(outgoing[edge_pointer[s]]) : outgoing[edge_pointer[s]];
            if(is_deleted_edge_[e_ind]){
                ++ edge_pointer[s];
                continue;
            }
        
            ui64 pushed = lvl_net_.pushFlow(e_ind, excess_[s]);
            size_t exc_to = push_back ? lvl_net_.getEdge(e_ind).from : lvl_net_.getEdge(e_ind).to;
            excess_[s] -= pushed;
            excess_[exc_to] += pushed;
            potential_out_[lvl_net_.getEdge(e_ind).from] -= pushed;
            potential_in_[lvl_net_.getEdge(e_ind).to] += pushed;
            next_vertices.push(lvl_net_.getEdge(e_ind).to);
            if(excess_[s] == 0) break;
            else ++edge_pointer[s];
    
            if(std::min(potential_in_[s],potential_out_[s]) == 0)
            {
                null_potential_.push_back(s);
            }
    
        }
    }
}


void BlockPreflowMKM::findMaxFlow(size_t s, size_t t)
{
    start_ = s;
    finish_ = t;
    
    while(true) // main cycle
    {
        vector<ui64> dist;
        network_.countDist(start_, dist);
        
        if(dist[finish_] == INF) break;
        
        makeLvlNet_(dist);
        initPotential_();
        
        potential_in_[start_] = INF;
        potential_out_[finish_] = INF;
        null_potential_.reserve(lvl_net_.getNetworkSizeV());
        
        for(size_t i = 0 ; i < lvl_net_.getNetworkSizeV(); ++i) {
            if(std::min(potential_out_[i],potential_in_[i]) == 0) {
                bfsDeleteEmptyNodes_(i);
            }
        }
        

        while(true) //find block flow
        {
            //find min potential
            
            null_potential_.resize(0);
            
            ui64 min_potential = INF;
            size_t min_ind = start_;
            for(size_t i = 0; i < lvl_net_.getNetworkSizeV(); ++i)
            {
                if(is_deleted_vertex_[i] || i == finish_) continue;
            
                if(min_potential > std::min(potential_out_[i],potential_in_[i]))
                {
                    min_potential = std::min(potential_out_[i],potential_in_[i]);
                    min_ind = i;
                }
            }
            if(min_potential == INF || min_potential == 0) break;
            
            excess_[min_ind] = min_potential;
            assert(excess_[min_ind] <= std::min(potential_out_[min_ind],potential_in_[min_ind]));
            pushFlow_(min_ind,false);

            excess_[min_ind] = min_potential;
            pushFlow_(min_ind,true);
            
            for(size_t i = 0 ; i < null_potential_.size(); ++i) {
                if(!is_deleted_vertex_[null_potential_[i]]) {
                    bfsDeleteEmptyNodes_(null_potential_[i]);
                }
            }
        }

        // add new flow to the base network
        for(size_t i = 0; i < lvl_net_.getNetworkSizeE(); ++i)
        {
            if(lvl_net_.isBackEdge(i)) continue;
            network_.pushFlow(associated_[i], lvl_net_.getEdgeFlow(i));
        }
    }
}

