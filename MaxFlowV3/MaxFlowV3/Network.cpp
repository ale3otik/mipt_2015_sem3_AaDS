//
//  Network.cpp
//  MaxFlowV3
//
//  Created by Alex on 12.10.15.
//  Copyright © 2015 Alex. All rights reserved.
//

#include <queue>
#include "MainHeader.h"

Network::Network():Graph(),max_flow(-1){}
Network::Network(const Graph & graph):
Graph(graph),
max_flow(-1)
{
    flow.resize(edges.size());
    for (int i = 0; i < flow.size(); ++i)
    {
        flow[i] = 0;
    }
}

int Network::getEdgeFlow(int index)
{
    return flow[index];
}

void Network::bfs(int start, vector<int> & info) const// dist / last edge
{
    info.resize(vertex.size(),-1);
    
    queue<int> bfs_queue;
    info[start] = -1;
    bfs_queue.push(start);
    
    while(!bfs_queue.empty())
    {
        Vertex cur_ver = vertex[bfs_queue.front()];
        bfs_queue.pop();
        
        bool is_incoming = false;
        
        vector<int>::iterator edge_it;
        if(cur_ver.outgoing.size() > 0)
        {
            edge_it = cur_ver.outgoing.begin();
        }
        else
        {
            edge_it = cur_ver.incoming.begin();
            is_incoming = true;
        }
        
        while(edge_it != cur_ver.incoming.end())
        {
            Edge cur_edge = edges[*edge_it];
            
            bool is_allowed = false;
            int to;
            int from;
            if(!is_incoming)
            {
                to = cur_edge.to;
                from = cur_edge.from;
                if(cur_edge.capacity - flow[*edge_it] > 0) is_allowed = true;
            }
            else
            {
                to = cur_edge.from;
                from = cur_edge.to;
                if(flow[*edge_it] > 0) is_allowed = true;
            }
            
            if(is_allowed && (info[to] == -1))
            {
                info[to] = info[from] + 1;
                bfs_queue.push(to);
            }
            
            ++edge_it;
            if(!is_incoming && edge_it == cur_ver.outgoing.end())
            {
                edge_it = cur_ver.incoming.begin();
                is_incoming = true;
            }
        }
    }
}