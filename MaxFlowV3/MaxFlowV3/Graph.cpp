//
//  Graph.cpp
//  MaxFlowV3
//
//  Created by Alex on 12.10.15.
//  Copyright © 2015 Alex. All rights reserved.
//

#include "MainHeader.h"
#include <vector>
#include <exception>



Edge::Edge(int from, int to, int capacity)
{
    this->from = from;
    this->to = to;
    this->capacity = capacity;
}

Graph::Graph(){}
Graph::Graph(int vertex_quantity, vector <Edge> edges )
{
    this->edges = edges;
    vertex.resize(vertex_quantity);
    
    for (int i = 0; i < edges.size(); ++i)
    {
        vertex[edges[i].from].outgoing.push_back(i);
        vertex[edges[i].to].incoming.push_back(i);
    }
}

Edge Graph::getEdge(int position)
{
    if(position >= 0 && position < edges.size())
    {
        return edges[position];
    }
    throw out_of_range("error getEdge index");
}

int Graph::getCapacity(int position)
{
    return edges[position].capacity;
}
