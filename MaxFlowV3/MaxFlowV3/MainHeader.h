//
//  MainHeader.h
//  MaxFlowV3
//
//  Created by Alex on 12.10.15.
//  Copyright © 2015 Alex. All rights reserved.
//

#ifndef MainHeader_h
#define MainHeader_h

#include <vector>
#include <queue>
#include <iostream>
#include <assert.h>

static const long long INF = 1e18;
/**************************
 * base structures
 */
class Edge
{
public:
    Edge();
    Edge(size_t from, size_t to, unsigned long long capacity);
    bool operator < (const Edge & sc) const;
    size_t from;
    size_t to;
    unsigned long long capacity;
};

class Vertex
{
public:
    std::vector<size_t> outgoing;
};

class Graph
{
public:
    Graph();
    Graph(size_t vertex_quantity);
    Graph(size_t vertex_quantity, const std::vector<Edge> & edges);
    const Edge & getEdge(size_t pos) const;
    unsigned long long getCapacity(size_t pos) const;
    size_t addNewEdge(const Edge & new_edge);
    
protected:
    std::vector <Edge> edges_;
    std::vector <Vertex> vertex_;
};


/******************************************
 * network to maxflow-find algorithms
 */
class Network: public Graph
{
public:
    Network();
    Network(size_t vertex_quantity);
    Network(const Graph & graph);
    
    void setMaxFlow(unsigned long long new_max_flow);
    unsigned long long getMaxFlow() const;
    unsigned long long countCurrentFlow(size_t s) const;
    
    unsigned long long getEdgeFlow(size_t e_ind) const;
    void setEdgeFlow(size_t e_ind, unsigned long long new_flow);
    unsigned long long pushFlow(size_t ind, unsigned long long value); // return excess
    void setFlow(size_t ind, unsigned long long value);
    unsigned long long getAllowedCapacity(size_t ind) const;
    size_t backEdge(size_t ind) const;
    
    size_t getNetworkSizeV() const;
    const std::vector<size_t> & getOutgoingEdges(size_t v_ind) const;
    const Edge & getEdge(size_t e_ind) const;
    size_t createNewEdgeFromNetwork(size_t e_ind, const Network & net);
private:
    std::vector <unsigned long long> flow_; // flow on edges
    long long max_flow_; // value of max flow
    std::vector<bool> is_back_edge_;
    std::vector<size_t> back_edge_ind_; // to associate real and imaginary edges
    void bfs_(size_t start, std::vector<unsigned long long> & dist) const;
};

/******************************************
 * Simple-For push preflow algorithm
 */

class SimpleForAlg
{
public:
    SimpleForAlg(Graph graph);
    void findMaxFlow(size_t s, size_t t);
    const Network & returnNetwork() const;
    unsigned long long getValueOfMaxFlow() const;
    
private:
    Network network_;
    std::vector<unsigned long long> height_;
    std::vector<unsigned long long> excess_;
    std::vector<size_t> cur_edge_to_discharge_;
    size_t start_;
    size_t finish_;
    
    void discharge_(size_t v_ind);
    void relabel_(size_t v_ind);
    void push_(size_t e_ind);
    void setValueOfMaxFlow_();
};

/******************************************
 *
 */
class BlockPreflowMKM
{
public:
    BlockPreflowMKM(Graph graph);
    void findMaxFlow(size_t s, size_t t);
    Network returnNetwork() const;
    unsigned long long getValueOfMaxFlow() const;
    
private:
    Network network_;
    Network lvl_net_;
    std::vector<unsigned long long> potential_in_;
    std::vector<unsigned long long> potential_out_;
    std::vector<unsigned long long> excess_;
    std::vector<bool> is_deleted_edge_;
    std::vector<bool> is_deleted_vertex_;
    std::vector<size_t> out_edge_pointer_;
    std::vector<size_t> in_edge_pointer_;
    std::vector<size_t> associated_;
    std::vector<size_t> null_potential_;
    size_t start_;
    size_t finish_;
    
    void makeLvlNet_(const std::vector <size_t> & dist);
    void initPotential_();
    void dfsDeleteEmptyNodes_(size_t i);
    void pushFlowForward_(size_t s);
    void pushFlowBack_(size_t s);
    void countValueOfMaxFlow_();
};


#endif /* MainHeader_h */
