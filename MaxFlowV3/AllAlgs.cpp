#include <vector>
#include <queue>
#include <iostream>
#include <assert.h>


using std::vector;
typedef unsigned long long ui64;
typedef long long i64;
using std::queue;
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
    void buildBackEdgesAndFlowInfo(); //ONLY IF THE BACK EDGES DONOT EXIST
    
    bool isBackEdge(size_t e_ind) const;
    std::vector<unsigned long long> countDist(size_t start) const;
    
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
class BlockPreflowMKM
{
public:
    BlockPreflowMKM(Graph graph);
    void findMaxFlow(size_t s, size_t t);
    Network returnNetwork() const;
    
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
    
    void makeLvlNet_(const std::vector <unsigned long long> & dist);
    void initPotential_();
    void findBlockinFlowMKM_();
    void pushBlockingFlow_();
    void bfsDeleteEmptyNodes_(size_t i);
    void pushFlow_(size_t s, bool push_back);
};

/*************************EDGES*****************************************/
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

const Edge & Graph::getEdge(size_t pos) const {
    return edges_[pos];
}

ui64 Graph::getCapacity(size_t pos) const {
    return edges_[pos].capacity;
}

size_t Graph::addNewEdge(const Edge & new_edge) {
    edges_.push_back(new_edge);
    vertex_[new_edge.from].outgoing.push_back(edges_.size() - 1);
    return edges_.size() - 1;
}

/*********************Network*********************************/
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
/******************SimpleFor*****************************/
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

/******************************MKM************************************/
BlockPreflowMKM::BlockPreflowMKM(Graph graph):
network_(graph) {
    out_edge_pointer_.resize(network_.getNetworkSizeV());
    in_edge_pointer_.resize(network_.getNetworkSizeV());
    excess_.resize(network_.getNetworkSizeV());
    is_deleted_vertex_.resize(network_.getNetworkSizeV());
    is_deleted_edge_.reserve(network_.getNetworkSizeE());
    associated_.reserve(network_.getNetworkSizeE());
    null_potential_.reserve(network_.getNetworkSizeV());
}
Network BlockPreflowMKM::returnNetwork() const{
    return this->network_;
}

/***************************************************/

void BlockPreflowMKM::makeLvlNet_(const vector<ui64> & dist) {
    lvl_net_ = Network(network_.getNetworkSizeV());
    out_edge_pointer_.assign(network_.getNetworkSizeV(),0);
    in_edge_pointer_.assign(network_.getNetworkSizeV(),0);
    is_deleted_vertex_.assign(network_.getNetworkSizeV(),false);
    associated_.clear();
    excess_.assign(network_.getNetworkSizeV(),0);
    for(size_t v = 0; v < network_.getNetworkSizeV(); ++v) {
        const vector<size_t> & outgoing = network_.getOutgoingEdges(v);
        for(size_t e = 0; e < outgoing.size(); ++e) {
            size_t e_ind = outgoing[e];
            const Edge & e_cur = network_.Graph::getEdge(e_ind);
            
            if(dist[e_cur.to] == dist[v] + 1 && dist[e_cur.to] <= dist[finish_]
               && network_.getAllowedCapacity(e_ind) > 0)
            {
                lvl_net_.addNewEdge(Edge(e_cur.from,e_cur.to,network_.getAllowedCapacity(e_ind)));
                associated_.push_back(e_ind);
            }
        }
    }
    lvl_net_.buildBackEdgesAndFlowInfo();
    is_deleted_edge_.clear();
    is_deleted_edge_.resize(lvl_net_.getNetworkSizeE(),false);
}

void BlockPreflowMKM::initPotential_() {
    potential_in_.assign(network_.getNetworkSizeV(),0);
    potential_out_.assign(network_.getNetworkSizeV(),0);
    
    for(size_t v = 0 ; v < lvl_net_.getNetworkSizeV(); ++v) {
        const vector<size_t> & outgoing = lvl_net_.getOutgoingEdges(v);
        
        for(size_t e = 0; e < outgoing.size(); ++ e) {
            size_t e_ind = outgoing[e];
            if(lvl_net_.isBackEdge(e_ind)) {
                potential_in_[v] += lvl_net_.getAllowedCapacity(lvl_net_.backEdge(e_ind));
            }
            else {
                potential_out_[v] += lvl_net_.getAllowedCapacity(e_ind);
            }
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
                if(!lvl_net_.isBackEdge(e_ind)){
                    potential_in_[e.to] -= lvl_net_.getAllowedCapacity(e_ind);
                }
                else {
                    potential_out_[e.to] -= lvl_net_.getAllowedCapacity(lvl_net_.backEdge(e_ind));
                }
                if(std::min(potential_in_[e.to],potential_out_[e.to]) == 0) {
                    next_deleted.push(e.to);
                }
            }
            is_deleted_edge_[e_ind] = true;
            is_deleted_edge_[lvl_net_.backEdge(e_ind)] = true;
        }
        is_deleted_vertex_[s] = true;
    }
}

void BlockPreflowMKM::pushFlow_(size_t s, bool push_back) {
    queue<size_t> next_vertices;
    next_vertices.push(s);
    while(!next_vertices.empty()) {
        s = next_vertices.front();
        next_vertices.pop();
        if((s == finish_ && !push_back) || (s == start_ && push_back)) continue;
        const vector<size_t> & outgoing = lvl_net_.getOutgoingEdges(s);

        vector<size_t> & edge_pointer = push_back ? in_edge_pointer_ : out_edge_pointer_;
    
        // to fin
        while(excess_[s] > 0 && edge_pointer[s] < outgoing.size()) {
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
            potential_in_[lvl_net_.getEdge(e_ind).to] -= pushed;
          
            next_vertices.push(exc_to);
            
            if(excess_[s] == 0) break;
            else ++edge_pointer[s];
    
        }
        if(std::min(potential_in_[s],potential_out_[s]) == 0) {
            null_potential_.push_back(s);
        }
    }
}

void BlockPreflowMKM::pushBlockingFlow_() {
    while(true) {
        //find min potential
        null_potential_.clear();
        ui64 min_potential = INF;
        size_t min_ind = start_;
        for(size_t i = 0; i < lvl_net_.getNetworkSizeV(); ++i) {
            if(is_deleted_vertex_[i] || i == finish_) continue;
            
            if(min_potential > std::min(potential_out_[i],potential_in_[i])) {
                min_potential = std::min(potential_out_[i],potential_in_[i]);
                min_ind = i;
            }
        }
        
        if(min_potential == INF || min_potential == 0) break;
        
        excess_[min_ind] = min_potential;
        pushFlow_(min_ind,false);
        
        excess_[min_ind] = min_potential;
        pushFlow_(min_ind,true);
        
        for(size_t i = 0 ; i < null_potential_.size(); ++i) {
            if(!is_deleted_vertex_[null_potential_[i]]) {
                bfsDeleteEmptyNodes_(null_potential_[i]);
            }
        }
    }
}

void BlockPreflowMKM::findBlockinFlowMKM_() {
    initPotential_();
    
    potential_in_[start_] = INF;
    potential_out_[finish_] = INF;

    for(size_t i = 0 ; i < lvl_net_.getNetworkSizeV(); ++i) {
        if(std::min(potential_out_[i],potential_in_[i]) == 0) {
            bfsDeleteEmptyNodes_(i);
        }
    }
    pushBlockingFlow_();
}

void BlockPreflowMKM::findMaxFlow(size_t s, size_t t) {
    start_ = s;
    finish_ = t;
    while(true) {
        vector<ui64> dist = network_.countDist(start_);
        if(dist[finish_] == INF) break;

        makeLvlNet_(dist);
       
        findBlockinFlowMKM_();
        
        // add new flow to the base network
        for(size_t i = 0; i < lvl_net_.getNetworkSizeE(); ++i) {
            if(lvl_net_.isBackEdge(i)) continue;
            network_.pushFlow(associated_[i], lvl_net_.getEdgeFlow(i));
        }
    }
}

/***********************MAIN*************************************/
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
    // BlockPreflowMKM alg(graph);

    alg.findMaxFlow(0, n-1);
    
    Network net = alg.returnNetwork();
    std::cout << net.countCurrentFlow(0)<< std::endl;
    for(size_t i = 0;i < m; ++i) {
        std::cout <<net.getEdgeFlow(i) << std::endl;
    }
    
    return 0;
}

