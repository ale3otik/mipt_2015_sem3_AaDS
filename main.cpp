//
//  main.cpp
//  MaxFlow_EdmundsKarp
//
//  Created by Alex on 11.09.15.
//

#include <vector>
#include <iostream>
#include <stdio.h>
#include "MaxFlow_EdmundsKarp.h"
using namespace std;
int main(int argc, const char * argv[])
{
    int n,m;
    cin>>n>>m;
    vector<vector <int> > edges(m,vector<int>(3));
    for(int i = 0; i < m; ++i)
    {
        cin>>edges[i][0]>>edges[i][1]>>edges[i][2];
    }
    
    Graph graph(edges,n);
    graph.FindMaxFlow(1,n);

    cout<<graph.GetLastCountedMaxFlow()<<endl;
    return 0;
}
