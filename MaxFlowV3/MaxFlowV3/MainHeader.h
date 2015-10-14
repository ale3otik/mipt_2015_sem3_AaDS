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
#include <exception>
using namespace std;

/**************************
 * base structures
 */
class Edge
{
public:
    Edge(int from,int to, int capacity);
    bool operator < (const Edge & sc) const;
    int from;
    int to;
    int capacity;
};

class Vertex
{
public:
    vector<int> incoming;
    vector<int> outgoing;
};

class Graph
{
public:
    Graph();
    Graph(int vertex_quantity, vector <Edge> edges );
    Edge getEdge(int position);
    int getCapacity(int position);
    
protected:
    vector <Edge> edges;
    vector<Vertex> vertex;
};


/******************************************
 * network to maxflow-find algorithms
 */
class Network: public Graph
{
public:
    friend class SimpleForAlg;
    friend class BlockPreflowMKM;
    
    Network();
    Network(const Graph & graph);
    int getEdgeFlow(int index);
private:
    vector <int> flow; // flow on edges
    long long max_flow; // value of max flow
    
    void bfs(int start, vector<int> & info) const;
};

/******************************************
 * Simple-For push preflow algorithm
 */

class SimpleForAlg
{
public:
    SimpleForAlg(Graph graph);
    void findMaxFlow(int s, int t);
    Network returnNetwork() const;
    long long getValueOfMaxFlow() const;
    
private:
    Network network;
    vector<int> height;
    vector<long long> excess;
    int start;
    int finish;
    
    void discharge(int vertex);
    void relabel(int vertex);
    void push(int e_ind, int from);
    void countValueOfMaxFlow();
};

/******************************************
 *
 */
class BlockPreflowMKM
{
public:
    BlockPreflowMKM(Graph graph);
    void findMaxFlow(int s, int t);
    Network returnNetwork() const;
    long long getValueOfMaxFlow() const;
    
private:
    Network network;
    Network lvl_net;
    vector<long long> potential_in;
    vector<long long> potential_out;
    vector<long long> excess;
    vector<bool> is_deleted_edge;
    vector<bool> is_deleted_vertex;
    vector<int> out_edge_pointer;
    vector<int> in_edge_pointer;
    vector<int> associated;
    long long max_flow;
    int start;
    int finish;
    
    void makeLvlNet(vector <int> & bfs_info);
    void initPotential();
    void dfsDeleteEmptyNodes(int i);
    void dfsPushFlow(int s);
};


#endif /* MainHeader_h */
