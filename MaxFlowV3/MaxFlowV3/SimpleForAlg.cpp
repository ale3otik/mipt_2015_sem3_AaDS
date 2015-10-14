//
//  SimpleForAlg.cpp
//  MaxFlowV3
//
//  Created by Alex on 12.10.15.
//  Copyright © 2015 Alex. All rights reserved.
//
#include "MainHeader.h"

SimpleForAlg::SimpleForAlg(Graph graph):
network(graph){}

long long SimpleForAlg::getValueOfMaxFlow() const
{
    return network.max_flow;
}
Network SimpleForAlg::returnNetwork() const
{
    return network;
}

void SimpleForAlg::countValueOfMaxFlow()
{
    network.max_flow = 0;
    for(int i = 0; i < network.vertex[start].outgoing.size(); ++i)
    {
        network.max_flow += network.flow[network.vertex[start].outgoing[i]];
    }
    for(int i = 0; i < network.vertex[start].incoming.size(); ++i)
    {
        network.max_flow -= network.flow[network.vertex[start].incoming[i]];
    }
}

void SimpleForAlg::findMaxFlow(int st, int fin)
{
    start = st;
    finish = fin;
    height.resize(network.vertex.size());
    excess.resize(network.vertex.size());
    
    for (int v = 0; v < network.vertex.size(); ++v)
    {
        height[v] = 0;
        excess[v] = 0;
    }
    height[start] = (int)network.vertex.size();
    
    //start init
    for(int i = 0 ; i < network.vertex[start].outgoing.size(); ++i)
    {
        int e_ind = network.vertex[start].outgoing[i];
        Edge cur_edge = network.edges[e_ind];
        
        excess[cur_edge.to] += cur_edge.capacity;
        network.flow[e_ind] = cur_edge.capacity;
    }
    excess[finish] = 0;
    excess[start] = 0;

    bool is_updated = true;
    while (is_updated)
    {
        is_updated = false;
        for (int v_ind = 0; v_ind < network.vertex.size(); ++v_ind)
        {
            if(excess[v_ind] > 0)
            {
                is_updated = true;
                discharge(v_ind);
            }
        }
    }
    
    countValueOfMaxFlow();
}

void SimpleForAlg::discharge(int v_ind)
{
    bool is_incoming = false;
    vector<int>::iterator current = network.vertex[v_ind].outgoing.begin();
    while(excess[v_ind] > 0)
    {
        if(current == network.vertex[v_ind].outgoing.end())
        {
            current = network.vertex[v_ind].incoming.begin();
            is_incoming = true;
        }
        
        if(current == network.vertex[v_ind].incoming.end())
        {
            current = current = network.vertex[v_ind].outgoing.begin();
            is_incoming = false;
            relabel(v_ind);
            continue;
        }
        
        int from, to , allowed_flow;
        
        if(!is_incoming)
        {
            to = network.edges[(*current)].to;
            from = network.edges[(*current)].from;
            allowed_flow = network.edges[(*current)].capacity - network.flow[(*current)];
        }
        else
        {
            to = network.edges[(*current)].from;
            from = network.edges[(*current)].to;
            allowed_flow = network.flow[(*current)];
        }
        
        if(height[from] == height[to] + 1 && allowed_flow > 0)
        {
            push((*current), from);
        }
        
        ++current;
    }
}

void SimpleForAlg::push(int e_ind, int from)
{
    Edge cur_edge = network.edges[e_ind];
    int to = cur_edge.to;
    if(cur_edge.from != from)
    {
        to = cur_edge.from;
    }

    long long allowed_flow;
    if(from == cur_edge.from)
    {
        allowed_flow = min((long long)(cur_edge.capacity - network.flow[e_ind]),excess[from]);
        network.flow[e_ind] += allowed_flow;
    }
    else if(from == cur_edge.to)
    {
        allowed_flow =min((long long)(network.flow[e_ind]),excess[from]);
        network.flow[e_ind] -= allowed_flow;
    }
    else {throw exception();}
    
    excess[to] += allowed_flow;
    excess[from] -= allowed_flow;
    
    if(to == finish || to == start) excess[to] = 0;
}

void SimpleForAlg::relabel(int v_ind)
{
    int min_h = 2 * (int)network.vertex.size() + 3;
    for(int i = 0; i < network.vertex[v_ind].outgoing.size();++i)
    {
        int e_ind = network.vertex[v_ind].outgoing[i];
        Edge cur_e = network.edges[e_ind];
        if(cur_e.capacity - network.flow[e_ind] > 0)
        {
            if(height[cur_e.to] < min_h) min_h = height[cur_e.to];
        }
    }
    
    for(int i = 0; i < network.vertex[v_ind].incoming.size();++i)
    {
        int e_ind = network.vertex[v_ind].incoming[i];
        Edge cur_e = network.edges[e_ind];
        if(network.flow[e_ind] > 0)
        {
            if(height[cur_e.from] < min_h) min_h = height[cur_e.from];
        }
    }
    
    height[v_ind] = min_h + 1;
}
