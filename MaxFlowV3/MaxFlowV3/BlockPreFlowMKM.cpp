//
//  BlockPreFlowMKM.cpp
//  MaxFlowV3
//
//  Created by Alex on 14.10.15.
//  Copyright © 2015 Alex. All rights reserved.
//

//#define NDEBUG

#include "MainHeader.h"
#include <assert.h>
const long long MAX_LONG = 1e9 * 1e6 + 10;
BlockPreflowMKM::BlockPreflowMKM(Graph graph):
network(graph),
lvl_net(){}
Network BlockPreflowMKM::returnNetwork() const{
    return this->network;
}
long long BlockPreflowMKM::getValueOfMaxFlow() const{
    return network.max_flow;
}

void BlockPreflowMKM::countValueOfMaxFlow()
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

/***************************************************/

void BlockPreflowMKM::makeLvlNet(vector <int> & bfs_info)
{
    lvl_net.vertex = vector<Vertex>(network.vertex.size());
    lvl_net.edges = vector<Edge>();
    out_edge_pointer = vector<int>(network.vertex.size(),0);
    in_edge_pointer = vector<int>(network.vertex.size(),0);
    is_deleted_vertex = vector<bool>(network.vertex.size(),false);
    associated.resize(0);
    excess = vector<long long>(network.vertex.size(),0);
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
                if(bfs_info[e_cur.to] == bfs_info[i] + 1 && bfs_info[e_cur.to] <= bfs_info[finish] && e_cur.capacity > network.flow[*e_it])
                {
                    lvl_net.edges.push_back(Edge(i,e_cur.to, e_cur.capacity - network.flow[*e_it]));
                    lvl_net.vertex[i].outgoing.push_back((int)lvl_net.edges.size() - 1);
                    associated.push_back(*e_it); // remember index of new edge in base network
                    
                    lvl_net.vertex[e_cur.to].incoming.push_back((int)lvl_net.edges.size() - 1);
                }
            }
            else
            {
                if(bfs_info[e_cur.from] == bfs_info[i] + 1 && bfs_info[e_cur.from] <= bfs_info[finish]&& network.flow[*e_it] > 0)
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
    lvl_net.flow = vector<int>(lvl_net.edges.size(),0);
    is_deleted_edge = vector<bool>(lvl_net.edges.size(),false);
}

void BlockPreflowMKM::initPotential()
{
    potential_in = vector<long long>(network.vertex.size(),0);
    potential_out = vector<long long>(network.vertex.size(),0);
    
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
    if(is_deleted_vertex[s] || s == start || s == finish) return;
    
    queue<int> next_deleted;
    for(int i = out_edge_pointer[s]; i < lvl_net.vertex[s].outgoing.size();++i)
    {
        int e_ind = lvl_net.vertex[s].outgoing[i];
        int to = lvl_net.edges[e_ind].to;
        
        if(!is_deleted_edge[e_ind])
        {
            potential_in[to] -= (lvl_net.edges[e_ind].capacity - lvl_net.flow[e_ind]);
            potential_out[s] -= (lvl_net.edges[e_ind].capacity - lvl_net.flow[e_ind]);
            if(!is_deleted_vertex[to] && min(potential_in[to],potential_out[to]) == 0)
            {
                if(to != finish) next_deleted.push(to);
            }
        }
        
        is_deleted_edge[e_ind] = true;
    }
    
    for(int i = in_edge_pointer[s]; i < lvl_net.vertex[s].incoming.size();++i)
    {
        int e_ind = lvl_net.vertex[s].incoming[i];
        int from = lvl_net.edges[e_ind].from;
        if(!is_deleted_edge[e_ind])
        {
            potential_out[from] -= (lvl_net.edges[e_ind].capacity - lvl_net.flow[e_ind]);
            potential_in[s] -= lvl_net.edges[e_ind].capacity - lvl_net.flow[e_ind];
            if(!is_deleted_vertex[from] && min(potential_in[from],potential_out[from]) == 0)
            {
               if(from != start) next_deleted.push(from);
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

void BlockPreflowMKM::pushFlowForward(int s)
{
    if(s == finish) return;
    
    Vertex cur_v = lvl_net.vertex[s];
    queue<int> next_vertices;

    // to fin
    while(excess[s] > 0 && out_edge_pointer[s] < cur_v.outgoing.size())
    {
        int e_ind = lvl_net.vertex[s].outgoing[out_edge_pointer[s]];
        if(is_deleted_edge[e_ind])
        {
            ++ out_edge_pointer[s];
            continue;
        }
        
        Edge cur_e = lvl_net.edges[e_ind];
        if(cur_e.capacity - lvl_net.flow[e_ind] > excess[s])
        {
            potential_out[s] -= excess[s];
            potential_in[cur_e.to] -= excess[s];
           
            excess[cur_e.to] += excess[s];
            lvl_net.flow[e_ind] += excess[s];
            excess[s] = 0;
            
            next_vertices.push(cur_e.to);
            break;
        }
        else
        {
            potential_out[s] -= cur_e.capacity - lvl_net.flow[e_ind];
            potential_in[cur_e.to] -= cur_e.capacity - lvl_net.flow[e_ind];
         
            excess[cur_e.to] += cur_e.capacity - lvl_net.flow[e_ind];
            excess[s] -= cur_e.capacity - lvl_net.flow[e_ind];
            
            lvl_net.flow[e_ind] = cur_e.capacity;
            
            is_deleted_edge[e_ind] = true;
            next_vertices.push(cur_e.to);
            ++out_edge_pointer[s];
        }
    }
    assert(excess[s] == 0);
    
    if(min(potential_in[s],potential_out[s]) == 0)
    {
        null_potential.push_back(s);
    }
    
    while (!next_vertices.empty())
    {
        pushFlowForward(next_vertices.front());
        next_vertices.pop();
    }
}

void BlockPreflowMKM::pushFlowBack(int s)
{
    
    if(s == start) return;
    
    Vertex cur_v = lvl_net.vertex[s];
    queue<int> next_vertices;
    
    // to start
    while(excess[s] > 0 && in_edge_pointer[s] < cur_v.incoming.size())
    {
        int e_ind = lvl_net.vertex[s].incoming[in_edge_pointer[s]];
        if(is_deleted_edge[e_ind])
        {
            ++ in_edge_pointer[s];
            continue;
        }
        
        Edge cur_e = lvl_net.edges[e_ind];
        if(cur_e.capacity - lvl_net.flow[e_ind] > excess[s])
        {
            potential_in[s] -= excess[s];
            potential_out[cur_e.from] -= excess[s];
            
            excess[cur_e.from] += excess[s];
            lvl_net.flow[e_ind] += excess[s];
            excess[s] = 0;
            
            next_vertices.push(cur_e.from);
            break;
        }
        else
        {

            potential_in[s] -= cur_e.capacity - lvl_net.flow[e_ind];
            potential_out[cur_e.from] -= cur_e.capacity - lvl_net.flow[e_ind];

            excess[cur_e.from] += cur_e.capacity - lvl_net.flow[e_ind];
            excess[s] -= cur_e.capacity - lvl_net.flow[e_ind];
            
            lvl_net.flow[e_ind] = cur_e.capacity;
            
            is_deleted_edge[e_ind] = true;
            next_vertices.push(cur_e.from);
            ++in_edge_pointer[s];
        }
    }
    
    assert(excess[s] == 0);
    
    if(min(potential_in[s],potential_out[s]) == 0)
    {
        null_potential.push_back(s);
    }
    
    while (!next_vertices.empty())
    {
        pushFlowBack(next_vertices.front());
        next_vertices.pop();
    }
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
        potential_in[start] = MAX_LONG;
        potential_out[finish] = MAX_LONG;
        
        for(int i = 0 ; i < lvl_net.vertex.size(); ++i)
        {
            if(min(potential_out[i],potential_in[i]) == 0)
            {
                dfsDeleteEmptyNodes(i);
            }
        }
        

        while(true) //find block flow
        {
            //find min potential
            
            null_potential.resize(0);
            
            long long min_potential = MAX_LONG;
            int min_ind = -1;
            for(int i = 0; i < lvl_net.vertex.size(); ++i)
            {
                if(is_deleted_vertex[i] || i == finish) continue;
            
                if(min_potential > min(potential_out[i],potential_in[i]))
                {
                    min_potential = min(potential_out[i],potential_in[i]);
                    min_ind = i;
                }
            }
            if(min_potential == MAX_LONG || min_potential == 0) break;
            
            excess[min_ind] = min_potential;
            if(excess[min_ind] > min(potential_out[min_ind],potential_in[min_ind]))
            {
                assert(0);
            }
            
            pushFlowForward(min_ind);

            excess[min_ind] = min_potential;
            pushFlowBack(min_ind);
            
            for(int i = 0 ; i < null_potential.size(); ++i)
            {
                if(!is_deleted_vertex[null_potential[i]])
                {
                    dfsDeleteEmptyNodes(null_potential[i]);
                }
            }
        }
        // add new flow to the base network
        for(int i = 0; i < lvl_net.edges.size(); ++i)
        {
            Edge associated_edge = network.edges[associated[i]];
            Edge cur_edge = lvl_net.edges[i];
            if(cur_edge.from == associated_edge.from)
            {
                network.flow[associated[i]] += lvl_net.flow[i];
            }
            else
            {
                network.flow[associated[i]] -= lvl_net.flow[i];
            }
        }
    }
    countValueOfMaxFlow();
}

