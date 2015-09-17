//
//  MaxFlow_EdmundsKarp
//
//  Created by Alex on 11.09.15.
//

#include "MaxFlow_EdmundsKarp.h"
using std::vector;
using std::queue;

Graph::Graph(vector< vector <int> > & edge_info,int get_num_of_vertex):
last_max_flow(-1),
num_of_vertex(get_num_of_vertex)
{
    edges.resize(edge_info.size());
    graph.resize(get_num_of_vertex);
    network.resize(get_num_of_vertex);

    for (int i = 0; i < edge_info.size(); ++ i)
    {
        edges[i].from = edge_info[i][0]-1;
        edges[i].to = edge_info[i][1]-1;
        edges[i].capacity = edge_info[i][2];
        
        graph[edges[i].from].push_back(i);
    }
}
int Graph::GetLastCountedMaxFlow() const {return last_max_flow;}

void Graph::MakeNewNetwork()
{
    for(int k = 0; k < graph.size(); ++k)
    {
        network[k] = graph[k];
    }
    for(int k = 0; k < edges.size(); ++k)
    {
        edges[k].virtual_capacity = edges[k].capacity;
        if(edges[k].virtual_capacity < 0) edges[k].virtual_capacity = 0;
//        edges[k].flow = 0;
        
        edges[k].back_edge = -1;
        int to = edges[k].to;
        int from = edges[k].from;
        for(int i = 0; i < network[to].size(); ++i) // to fast update back edge
        {
            if(edges[network[to][i]].to == from)
            {
                edges[k].back_edge = network[to][i];
                edges[network[to][i]].back_edge = k;
            }
        }
    }
}

void Graph::FindMaxFlow(int start,int final)
{
    --start;
    --final;

    last_max_flow = 0;
    
    if(start < 0 || final > num_of_vertex - 1) return;

    MakeNewNetwork();
    
    bool is_new_path = true;
    while(true)
    {
        vector<int> ancestor(num_of_vertex,-1);
        ancestor[start] = -2;
        is_new_path = BfsFindPath(ancestor,start, final);
        if(!is_new_path) break;

        /*find minimal residual capacity*/
        int min_cap = -1;
        for(int cur = final;;)
        {
            int e_rev = ancestor[cur];
            if(e_rev == -2) break; // is start vertex

            if(min_cap == -1) min_cap = edges[e_rev].virtual_capacity; // first edge
            else if(edges[e_rev].virtual_capacity < min_cap)
            {
                min_cap = edges[e_rev].virtual_capacity;
            }
            
            cur = edges[e_rev].from;
        }
        
        /*update edges and edges capacity*/
        for(int cur = final;;)
        {
            int e_rev = ancestor[cur];
            if(e_rev == -2) break; // is start vertex
            
            edges[e_rev].virtual_capacity -= min_cap;
            int back_edge = edges[e_rev].back_edge;
            if(back_edge == -1)
            {
                Edge new_back_edge;
                new_back_edge.from = edges[e_rev].to;
                new_back_edge.to = edges[e_rev].from;
                new_back_edge.virtual_capacity = min_cap;
                new_back_edge.back_edge = e_rev;
                new_back_edge.capacity = -1; //it's virtual edge
                edges.push_back(new_back_edge);
                int index_of_new_edge = (int)edges.size()-1;
                network[new_back_edge.from].push_back(index_of_new_edge);
            }
            else
            {
                edges[back_edge].virtual_capacity += min_cap;
            }
            
            cur = edges[e_rev].from;
        }
        last_max_flow += min_cap; //update flow
    }
    
}

bool Graph::BfsFindPath(vector<int> & ancestor,int start, int final)
{
    queue<int> vque;
    vque.push(start);

    bool is_end = false;
    
    while(vque.size() > 0)
    {
        int cur = vque.front();
        vque.pop();
        for(int i = 0; i < network[cur].size();++i)
        {
            int e_ind = network[cur][i];
            if(edges[e_ind].virtual_capacity == 0) continue; // this edge is not from residual network
            
            if(ancestor[edges[e_ind].to] == -1)
            {
                ancestor[edges[e_ind].to] = e_ind;
                vque.push(edges[e_ind].to);
            }
            if(edges[e_ind].to == final)
            {
                is_end = true;
                break;
            }
        }
        if(is_end) break;
    }
    if(is_end) return true;
    return false;
}


