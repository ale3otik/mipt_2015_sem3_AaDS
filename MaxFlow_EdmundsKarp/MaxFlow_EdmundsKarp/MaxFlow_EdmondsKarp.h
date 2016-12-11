//
//  MaxFlow_EdmundsKarp.h
//  MaxFlow_EdmundsKarp
//
//  Created by Alex on 11.09.15.

#include <vector>
#include <queue>
#ifndef MaxFlow_EdmundsKarp_MaxFlow_EdmundsKarp_h
#define MaxFlow_EdmundsKarp_MaxFlow_EdmundsKarp_h

struct Edge
{
    int from;
    int to;
    int capacity;
    int virtual_capacity;
    int back_edge;
};

class Graph
{
public:
    Graph(std::vector<std::vector <int> > & edge_info, int num_of_nodes);
    void FindMaxFlow(int start,int final);
    int GetLastCountedMaxFlow() const;
    
private:
    void MakeNewNetwork();
    bool BfsFindPath(std::vector<int> & ancestor, int start, int final);
    
    std::vector< std::vector<int>> graph;
    std::vector< std::vector<int>> network;
    std::vector<Edge> edges;
    int last_max_flow;
    const int num_of_vertex;
};

#endif
