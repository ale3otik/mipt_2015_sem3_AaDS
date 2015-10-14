//
//  BlockPreFlowMKM.cpp
//  MaxFlowV3
//
//  Created by Alex on 14.10.15.
//  Copyright © 2015 Alex. All rights reserved.
//

#include "MainHeader.h"

BlockPreflowMKM::BlockPreflowMKM(Graph graph):
network(graph),
lvl_net(){}
Network BlockPreflowMKM::returnNetwork() const{
    return this->network;
}
long long BlockPreflowMKM::getValueOfMaxFlow() const{
    return this->max_flow;
}

/***************************************************/

void BlockPreflowMKM::makeLvlNet(vector <int> & bfs_info)
{
    lvl_net.vertex.resize(network.vertex.size());
    out_edge_pointer.resize(network.vertex.size(),0);
    in_edge_pointer.resize(network.vertex.size(),0);
    is_deleted_vertex.resize(network.vertex.size(),false);
    excess.resize(network.vertex.size(),0);
    for(int i = 0; i < network.vertex.size(); ++i)
    {
        Vertex v_last = network.vertex[i];
        vector<int>::iterator e_it;
        bool is_incoming = false;
        if(v_last.outgoing.size() > 0)
        {
            e_it = v_last.outgoing.begin();
        }
        else
        {
            e_it = v_last.incoming.begin();
            is_incoming = true;
        }
        
        while(e_it != v_last.incoming.end())
        {
            Edge e_cur = network.edges[*e_it];
            if(!is_incoming)
            {
                if(bfs_info[e_cur.to] == bfs_info[i] + 1 && e_cur.capacity > network.flow[*e_it])
                {
                    lvl_net.edges.push_back(Edge(i,e_cur.to, e_cur.capacity - network.flow[*e_it]));
                    lvl_net.vertex[i].outgoing.push_back((int)lvl_net.edges.size() - 1);
                    associated.push_back(*e_it); // remember index of new edge in base network
                    
                    lvl_net.vertex[e_cur.to].incoming.push_back((int)lvl_net.edges.size() - 1);
                }
            }
            else
            {
                if(bfs_info[e_cur.from] == bfs_info[i] + 1 && network.flow[*e_it] > 0)
                {
                    lvl_net.edges.push_back(Edge(i,e_cur.from,network.flow[*e_it]));
                    lvl_net.vertex[i].outgoing.push_back((int)lvl_net.edges.size() - 1);
                    associated.push_back(*e_it);
                    
                    lvl_net.vertex[e_cur.from].incoming.push_back((int)lvl_net.edges.size() - 1);
                }
            }
            
            ++e_it;
            if(!is_incoming && e_it == v_last.outgoing.end())
            {
                e_it = v_last.incoming.begin();
                is_incoming = true;
            }
        }
    }
    lvl_net.flow.resize(lvl_net.edges.size(),0);
    is_deleted_edge.resize(lvl_net.edges.size(),0);
}

void BlockPreflowMKM::initPotential()
{
    potential_in.resize(network.vertex.size(),0);
    potential_out.resize(network.vertex.size(),0);
    
    for(int i = 0 ;i < lvl_net.vertex.size(); ++i)
    {
        for(int j = 0; j < lvl_net.vertex[i].incoming.size(); ++j )
        {
            potential_in[i] += lvl_net.edges[lvl_net.vertex[i].incoming[j]].capacity;
        }
        for(int j = 0; j < lvl_net.vertex[i].outgoing.size(); ++j)
        {
            potential_out[i] += lvl_net.edges[lvl_net.vertex[i].outgoing[j]].capacity;
        }
    }
}

void BlockPreflowMKM::dfsDeleteEmptyNodes(int s)
{
    if(is_deleted_vertex[s]) return;
    
    queue<int> next_deleted;
    for(int i = out_edge_pointer[s]; i < lvl_net.vertex[s].outgoing.size();++i)
    {
        int e_ind = lvl_net.vertex[s].outgoing[i];
        int to = lvl_net.edges[e_ind].to;
        
        if(to != finish && !is_deleted_edge[e_ind])
        {
            potential_in[to] -= lvl_net.edges[e_ind].capacity;
            if(!is_deleted_vertex[to] && min(potential_in[to],potential_out[to]) == 0)
            {
                next_deleted.push(to);
            }
        }
        
        is_deleted_edge[e_ind] = true;
    }
    
    for(int i = in_edge_pointer[s]; i < lvl_net.vertex[s].incoming.size();++i)
    {
        int e_ind = lvl_net.vertex[s].incoming[i];
        int from = lvl_net.edges[e_ind].from;
        if(from != start && !is_deleted_edge[e_ind])
        {
            potential_out[from] -= lvl_net.edges[e_ind].capacity;
            if(!is_deleted_vertex[from] && min(potential_in[from],potential_out[from]) == 0)
            {
                next_deleted.push(from);
            }
        }
        
        is_deleted_edge[e_ind] = true;
    }
    
    is_deleted_vertex[s] = true;
    
    while (!next_deleted.empty())
    {
        dfsDeleteEmptyNodes(next_deleted.front());
        next_deleted.pop();
    }
}

void BlockPreflowMKM::dfsPushFlow(int s)
{
    Vertex cur_v = lvl_net.vertex[s];
    
    vector<int> saturated;
    long long cur_excess = excess[s];
    // to fin
    while(cur_excess > 0 && out_edge_pointer[s] < cur_v.outgoing.size())
    {
        Edge cur_e = lvl_net.edges[out_edge_pointer[s]];
        if(cur_e.capacity - lvl_net.flow[out_edge_pointer[s]] > cur_excess)
        {
            lvl_net.flow[out_edge_pointer[s]] += cur_excess;
            
            cur_excess = 0;
            break;
        }
        else
        {
            
            saturated.push_back(out_edge_pointer[s]);
            ++out_edge_pointer[s];
        }
    }
    
    //back to start
    cur_excess = excess[s];
    while(cur_excess > 0 && in_edge_pointer[s] < cur_v.incoming.size())
    {
        
    }
    
    excess[s] = 0;
    
}

void BlockPreflowMKM::findMaxFlow(int s, int t)
{
    start = s;
    finish = t;
    
    while(true) // main cycle
    {
        vector<int> bfs_info; // dist
        network.bfs(start, bfs_info);
        
        if(bfs_info[finish] == -1) break;
        
        makeLvlNet(bfs_info);
        initPotential();
        for(int i = 0 ; i < lvl_net.vertex.size(); ++i)
        {
            dfsDeleteEmptyNodes(i);
        }
        
        while(true) //find block flow
        {
            //find min potential
            long long min_potential = network.edges.size() + 1;
            int min_ind = -1;
            for(int i = 0; i < lvl_net.vertex.size(); ++i)
            {
                if(i == start || i == finish || is_deleted_vertex[i]) continue;
            
                if(min_potential > min(potential_out[i],potential_in[i]))
                {
                    min_potential = min(potential_out[i],potential_in[i]);
                    min_ind = i;
                }
            }
            if(min_ind == -1) break;
            
            excess[min_ind] += min_potential;
            dfsPushFlow(min_ind);
        }
    }
}

