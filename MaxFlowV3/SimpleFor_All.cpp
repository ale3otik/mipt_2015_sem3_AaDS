#include <vector>
#include <queue>
#include <iostream>
#include <assert.h>

using std::queue;
using std::vector;

typedef unsigned long long ui64;
typedef long long i64;
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
    unsigned long long countCurrentFlow(size_t s) const;
    
    unsigned long long getEdgeFlow(size_t e_ind) const;
    void setEdgeFlow(size_t e_ind, unsigned long long new_flow);
    unsigned long long pushFlow(size_t ind, unsigned long long value); // return value of pushed flow
    void setFlow(size_t ind, unsigned long long value);
    unsigned long long getAllowedCapacity(size_t ind) const;
    size_t backEdge(size_t ind) const;
    
    size_t getNetworkSizeV() const;
    size_t getNetworkSizeE() const;
    const std::vector<size_t> & getOutgoingEdges(size_t v_ind) const;
    const Edge & getEdge(size_t e_ind) const;
    size_t createNewEdgeFromNetwork(size_t e_ind, const Network & net);
    
    bool isBackEdge(size_t e_ind) const;
    
    void countDist(size_t start, std::vector<unsigned long long> & dist) const;
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
};

/******************************************
 *
 */
/******************************************
 *
 */
/******************************************
 *
 */
Edge::Edge(){}

Edge::Edge(size_t from, size_t to, ui64 capacity){
    this->from = from;
    this->to = to;
    this->capacity = capacity;
}

Graph::Graph(){}

Graph::Graph(size_t vertex_quantity):
edges_(),
vertex_(vertex_quantity){}

Graph::Graph(size_t vertex_quantity, const vector <Edge> & edges) {
    this->edges_ = edges;
    vertex_.resize(vertex_quantity);
    
    for (int i = 0; i < edges.size(); ++i) {
        vertex_[edges[i].from].outgoing.push_back(i);
    }
}

const Edge & Graph::getEdge(size_t pos) const {
    return edges_[pos];
}

ui64 Graph::getCapacity(size_t pos) const {
    return edges_[pos].capacity;
}

size_t Graph::addNewEdge(const Edge & new_edge) {
    edges_.push_back(new_edge);
    return edges_.size() - 1;
}

/****************NETWORK*******************/

//
//  Network.cpp
//  MaxFlowV3
//
//  Created by Alex on 12.10.15.
//  Copyright © 2015 Alex. All rights reserved.
//

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
        vertex_[from].outgoing.push_back(i + j);
        is_back_edge_[i + j] = true;
        back_edge_ind_[i + j] = j;
        back_edge_ind_[j] = i;
     }
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


size_t Network::createNewEdgeFromNetwork(size_t e_ind, const Network & net) {
    const Edge parent_edge = net.getEdge(e_ind);
    Edge newEdge(parent_edge.from,parent_edge.to,net.getAllowedCapacity(e_ind));
    Edge newBackEdge(parent_edge.to,parent_edge.from,0);
    
    size_t indr = addNewEdge(newEdge);
    size_t indb = addNewEdge(newBackEdge);
    is_back_edge_.push_back(false);
    is_back_edge_.push_back(true);
    back_edge_ind_.push_back(indb);
    back_edge_ind_.push_back(indr);
    vertex_[parent_edge.from].outgoing.push_back(indr);
    vertex_[parent_edge.to].outgoing.push_back(indb);
    flow_.push_back(0);
    flow_.push_back(0);
    return indr;
}

unsigned long long Network::pushFlow(size_t ind, ui64 value) {
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

void Network::setEdgeFlow(size_t ind, unsigned long long value) {
    pushFlow(backEdge(ind), getEdgeFlow(ind)); // set 0
    pushFlow(ind, value);
}

unsigned long long Network::getAllowedCapacity(size_t ind) const {
    if(!is_back_edge_[ind]) return edges_[ind].capacity - flow_[ind];
    return flow_[backEdge(ind)];
}

unsigned long long Network::countCurrentFlow(size_t s) const {
    i64 cur_flow = 0;
    const Vertex & cur = vertex_[s];
    for(ui64 i = 0; i < cur.outgoing.size(); ++i) {
        size_t e_ind = cur.outgoing[i];
        if(!is_back_edge_[e_ind]) {
            cur_flow += getEdgeFlow(e_ind);
        }
         else {
            cur_flow -= getEdgeFlow(backEdge(e_ind));
        }
    }

    return (ui64)cur_flow;
}

void Network::countDist(size_t start, std::vector<ui64> & dist) const {
    bfs_(start, dist);
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

/****************SIMPLE FOR****************/
//
//  SimpleForAlg.cpp
//  MaxFlowV3
//
//  Created by Alex on 12.10.15.
//  Copyright © 2015 Alex. All rights reserved.
//


SimpleForAlg::SimpleForAlg(Graph graph):
network_(graph),
cur_edge_to_discharge_(network_.getNetworkSizeV(),0){}

const Network & SimpleForAlg::returnNetwork() const {
    return network_;
}

void SimpleForAlg::findMaxFlow(size_t st, size_t fin)
{
    start_ = st;
    finish_ = fin;

    height_.clear();
    excess_.clear();
    height_.resize(network_.getNetworkSizeV(),0);
    excess_.resize(network_.getNetworkSizeV(),0);

    height_[start_] = network_.getNetworkSizeV();
    
    //start init
    const vector<size_t> & outgoing = network_.getOutgoingEdges(start_);
    for(size_t i = 0 ; i < outgoing.size(); ++i)
    {
        size_t e_ind = outgoing[i];
        if(network_.isBackEdge(e_ind)) continue;
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
}

void SimpleForAlg::discharge_(size_t v_ind)
{
    size_t e_ind = cur_edge_to_discharge_[v_ind]; // to continue with last point
    const vector<size_t> & outgoing = network_.getOutgoingEdges(v_ind);
    while(excess_[v_ind] > 0) {
        if(e_ind == outgoing.size()){
            relabel_(v_ind);
            e_ind = 0;
            continue;
        }
        size_t e = outgoing[e_ind];
        const Edge & edge = network_.getEdge(e);
        if(height_[edge.from] == height_[edge.to] + 1 && network_.getAllowedCapacity(e) > 0) {
            push_(e);
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


/**************************************************************************/

int main()
{
    size_t n,m;

    std::cin>> n >> m;
    std::vector<Edge> edges(m,Edge(0, 0, 0));
    
    for(size_t i = 0; i < m; ++i)
    {
        std::cin >> edges[i].from;
        std::cin >> edges[i].to;
        std::cin >> edges[i].capacity;
        
        --edges[i].from;
        --edges[i].to;
    }

    Graph graph(n,edges);
    SimpleForAlg alg(graph);
    alg.findMaxFlow(0, n-1);
    
    Network net = alg.returnNetwork();
    std::cout << net.countCurrentFlow(0)<< std::endl;
    for(size_t i = 0;i < m; ++i) {
        std::cout <<net.getEdgeFlow(i) << std::endl;
    }
    
    return 0;
}
