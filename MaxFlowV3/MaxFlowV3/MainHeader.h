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
    Network(const Graph & graph);
    int getEdgeFlow(int index);
private:
    vector <int> flow; // flow on edges
    long long max_flow; // value of max flow
};

/******************************************
 * Simple-For push preflow algorithm
 */

class SimpleForAlg
{
public:
    SimpleForAlg(Graph graph);
    void findMaxFlow(int s, int t);
    Network returnNetwork();
    long long getValueOfMaxFlow();
    
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
class BlockPreflow
{
    
};


#endif /* MainHeader_h */
