#include "MainHeader.h"

int main()
{
    size_t n,m;

    std::cin>> n >> m;
    std::vector<Edge> edges(m,Edge(0, 0, 0));
    
    for(size_t i = 0; i < m; ++i)
    {
        std::cin >> edges[i].from;
        std::cin >> edges[i].to;
        std::cin >> edges[i].capacity;
        
        --edges[i].from;
        --edges[i].to;
    }

    Graph graph(n,edges);
    Network net(graph);
    
    BlockPreflowMKM alg(net);
    //SimpleForAlg alg(net);
    
    alg.findMaxFlow(0, n-1);
    
    std::cout << net.getMaxFlow()<< std::endl;
    for(size_t i = 0;i < m; ++i) {
        std::cout <<net.getEdgeFlow(i) << std::endl;
    }
    
    return 0;
}
