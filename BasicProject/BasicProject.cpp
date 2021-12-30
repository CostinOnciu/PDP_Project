#include <iostream>
#include "Graph.h"

void createGraph(Graph& g)
{
    for (int i = 0;i < g.size - 1;i++)
    {
        g.addEdge(i, i + 1);
    }
    
    g.addEdge(g.size - 1, 0);
    g.addEdge(0, 2);
}

void createRandomGraph()
{

}

void parallelBFSColouring()
{

}

int main()
{
    int size = 10;

    Graph graph = Graph(size);
    createGraph(graph);

    for (int i = 0;i < graph.size;i++)
    {
        std::cout << "\n";
        for (auto it = graph.neighbours[i].begin();it != graph.neighbours[i].end();it++)
            std::cout << *it << " ";
    }
}
