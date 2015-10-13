//
//  Network.cpp
//  MaxFlowV3
//
//  Created by Alex on 12.10.15.
//  Copyright © 2015 Alex. All rights reserved.
//

#include "MainHeader.h"

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