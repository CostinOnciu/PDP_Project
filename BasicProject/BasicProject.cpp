#include <iostream>
#include "Graph.h"
#include <boost/asio/thread_pool.hpp>
#include <boost/asio.hpp>
#include <queue>

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

void parallelBFSColouring(Graph& graph, int numberOfColours)
{
    boost::asio::thread_pool th_pool(8);

    std::vector<int> visited(graph.size, 0);

    int maxColors = 1;

    // Do a full BFS traversal from
    // all unvisited starting points
    for (int sv = 0; sv < graph.size; sv++)
    {

        if (visited[sv])
            continue;

        // If the starting point is unvisited,
        // mark it visited and push it in queue
        visited[sv] = 1;
        std::queue<int> q;
        q.push(sv);

        // BFS Travel starts here

        /*
            TODO:
            HERE ADD WORK TO THE POOL
        */
        // boost::asio::post(th_pool, []() {}); 
        while (!q.empty())
        {

            int top = q.front();
            q.pop();

            // Checking all adjacent nodes
            // to "top" edge in our queue
            for (auto it = graph.neighbours[top].begin();
                it != graph.neighbours[top].end(); it++)
            {

                // IMPORTANT: If the color of the
                // adjacent node is same, increase it by 1
                if (graph.colors[top] == graph.colors[*it])
                    graph.colors[*it] += 1;

                // If number of colors used shoots m, return
                // 0
                maxColors
                    = std::max(maxColors, std::max(graph.colors[top],
                        graph.colors[*it]));
                if (maxColors > numberOfColours)
                    throw ColouringException();

                // If the adjacent node is not visited,
                // mark it visited and push it in queue
                if (!visited[*it]) {
                    visited[*it] = 1;
                    q.push(*it);
                }
            }
        }
    }
}

int main()
{
    int size = 10;

    Graph graph = Graph(size);
    createGraph(graph);

    try
    {
        parallelBFSColouring(graph, 2);
        for (auto it = graph.colors.begin(); it != graph.colors.end(); ++it)
        {
            std::cout << *it << " ";
        }
    }
    catch (const ColouringException& ex)
    {
        std::cout << ex.what();
    }
}
