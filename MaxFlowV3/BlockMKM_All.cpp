#include <vector>
#include <queue>
#include <exception>
#include <assert.h>
#include <iostream>
using namespace std;

long long min(long long a, long long b)
{
    if(a < b) return a;
    return b;
}
/**************************
 * base structures
 */
class Edge
{
public:
    Edge(int from,int to, int capacity);
    bool operator < (const Edge & sc) const;
    int from;
    int to;
    int capacity;
};

class Vertex
{
public:
    Vertex():
    incoming(),
    outgoing(){}
    
    vector<int> incoming;
    vector<int> outgoing;
};

class Graph
{
public:
    Graph();
    Graph(int vertex_quantity, vector <Edge> edges );
    Edge getEdge(int position);
    int getCapacity(int position);
    
protected:
    vector <Edge> edges;
    vector<Vertex> vertex;
};


/******************************************
 * network to maxflow-find algorithms
 */
class Network: public Graph
{
public:
    friend class SimpleForAlg;
    friend class BlockPreflowMKM;
    
    Network();
    Network(const Graph & graph);
    int getEdgeFlow(int index);
private:
    vector <int> flow; // flow on edges
    long long max_flow; // value of max flow
    
    void bfs(int start, vector<int> & info) const;
};

/******************************************
 * Simple-For push preflow algorithm
 */

class SimpleForAlg
{
public:
    SimpleForAlg(Graph graph);
    void findMaxFlow(int s, int t);
    Network returnNetwork() const;
    long long getValueOfMaxFlow() const;
    
private:
    Network network;
    vector<int> height;
    vector<long long> excess;
    int start;
    int finish;
    
    void discharge(int vertex);
    void relabel(int vertex);
    void push(int e_ind, int from);
    void countValueOfMaxFlow();
};

/******************************************
 *
 */
class BlockPreflowMKM
{
public:
    BlockPreflowMKM(Graph graph);
    void findMaxFlow(int s, int t);
    Network returnNetwork() const;
    long long getValueOfMaxFlow() const;
    
private:
    Network network;
    Network lvl_net;
    vector<long long> potential_in;
    vector<long long> potential_out;
    vector<long long> excess;
    vector<bool> is_deleted_edge;
    vector<bool> is_deleted_vertex;
    vector<int> out_edge_pointer;
    vector<int> in_edge_pointer;
    vector<int> associated;
    vector<int> null_potential;
    queue<int> next_vertices;
    int start;
    int finish;
    
    void makeLvlNet(vector <int> & bfs_info);
    void initPotential();
    void dfsDeleteEmptyNodes(int i);
    void dfsPushFlowForward(int s);
    void dfsPushFlowBack(int s);
    void countValueOfMaxFlow();
};


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
    
    return edges[position];
    
    
}

int Graph::getCapacity(int position)
{
    return edges[position].capacity;
}

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
    info[start] = 0;
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

void BlockPreflowMKM::dfsPushFlowForward(int s)
{
    if(s == finish) return;
    
    Vertex cur_v = lvl_net.vertex[s];
    //queue<int> next_vertices;

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
        if((long long)cur_e.capacity - lvl_net.flow[e_ind] > excess[s])
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
    
    //while (!next_vertices.empty())
    //{
      //  dfsPushFlowForward(next_vertices.front());
        //next_vertices.pop();
    //}
}

void BlockPreflowMKM::dfsPushFlowBack(int s)
{
    
    if(s == start) return;
    
    Vertex cur_v = lvl_net.vertex[s];
    
    
    // to start
    //if(excess[s] > potential_in[s]) /////////////////////////////////////////////
   // {
      //  assert(0);
   // }
    while(excess[s] > 0 && in_edge_pointer[s] < cur_v.incoming.size())
    {
        int e_ind = lvl_net.vertex[s].incoming[in_edge_pointer[s]];
        if(is_deleted_edge[e_ind])
        {
            ++ in_edge_pointer[s];
            continue;
        }
        
        Edge cur_e = lvl_net.edges[e_ind];
        if((long long)(cur_e.capacity - lvl_net.flow[e_ind]) > excess[s])
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
    
    //while (!next_vertices.empty())
    //{
      //  dfsPushFlowBack(next_vertices.front());
        //next_vertices.pop();
    //}
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

            next_vertices.push(min_ind);
            int qsize = (int)next_vertices.size();
            while(qsize > 0)
            {
                for(int i = 0; i < qsize; ++i)
                {
                    dfsPushFlowForward(next_vertices.front());
                    next_vertices.pop();
                }
                qsize = (int)next_vertices.size();
            }
            
            excess[min_ind] = min_potential;
            
            next_vertices.push(min_ind);

            qsize = (int)next_vertices.size();
            while(qsize > 0)
            {
                for(int i = 0; i < qsize; ++i)
                {
                    dfsPushFlowBack(next_vertices.front());
                    next_vertices.pop();
                }
                qsize = (int)next_vertices.size();
            }
            
            for(int i = 0 ; i < null_potential.size(); ++i)
            {
                if(!is_deleted_vertex[null_potential[i]])
                {
                    dfsDeleteEmptyNodes(null_potential[i]);
                }
            }
            null_potential.clear();
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
