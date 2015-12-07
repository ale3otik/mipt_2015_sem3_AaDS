#include "MainHeader.h"

int main()
{
    int n,m;

    std::cin>> n >> m;
    std::vector<Edge> edges(m,Edge(0, 0, 0));
    
    for(int i = 0; i < m; ++i)
    {
        std::cin >> edges[i].from;
        std::cin >> edges[i].to;
        std::cin >> edges[i].capacity;
        
        --edges[i].from;
        --edges[i].to;
    }

    Graph graph(n,edges);
    BlockPreflowMKM alg(graph);
    alg.findMaxFlow(0, n-1);
    
    Network net = alg.returnNetwork();
    std::cout << alg.getValueOfMaxFlow() << std::endl;
    for(int i = 0;i < m; ++i)
    {
        std::cout <<net.getEdgeFlow(i) << std::endl;
    }
    return 0;
}
