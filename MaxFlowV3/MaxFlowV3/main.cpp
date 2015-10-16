

#include "MainHeader.h"

int main()
{
    int n,m;

    cin>> n >> m;
    vector<Edge> edges(m,Edge(0, 0, 0));
    
    for(int i = 0; i < m; ++i)
    {
        cin >> edges[i].from;
        cin >> edges[i].to;
        cin >> edges[i].capacity;
        
        --edges[i].from;
        --edges[i].to;
    }

    Graph graph(n,edges);
    BlockPreflowMKM alg(graph);
    alg.findMaxFlow(0, n-1);
    
    Network net = alg.returnNetwork();
    cout << alg.getValueOfMaxFlow() << endl;
    for(int i = 0;i < m; ++i)
    {
        cout <<net.getEdgeFlow(i) << endl;
    }
    return 0;
}
