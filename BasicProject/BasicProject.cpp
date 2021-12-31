#include <iostream>
#include "Graph.h"
#include <boost/asio/thread_pool.hpp>
#include <boost/asio.hpp>
#include <queue>
#include <memory>
#include <mutex>

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

void threadFunction(Graph& graph, int currentVertex, std::vector<int>& visited, int& maxColours, int numberOfColours, std::queue<int>& q, std::vector<std::mutex*> mutexes, std::mutex* q_mutex, bool& validNumberOfColors)
{
    bool locked = false;
    auto i = graph.neighbours[currentVertex].end();
    i--;
    for (auto it = graph.neighbours[currentVertex].begin();
        it != graph.neighbours[currentVertex].end(); it++)
    {
        
        if (!locked && *it > currentVertex)
        {
            mutexes[currentVertex]->lock();
            locked = true;
        }
        mutexes[*it]->lock();

        if (it == i && !locked) {
            mutexes[currentVertex]->lock();
            locked = true;
        }

        // IMPORTANT: If the color of the
        // adjacent node is same, increase it by 1
        if (graph.colors[currentVertex] == graph.colors[*it])
            graph.colors[*it] += 1;

        // If number of colors used shoots m, return
        // 0
        maxColours
            = std::max(maxColours, std::max(graph.colors[currentVertex],
                graph.colors[*it]));
        if (maxColours > numberOfColours)
            validNumberOfColors = false;

        // If the adjacent node is not visited,
        // mark it visited and push it in queue
        if (!visited[*it]) {
            visited[*it] = 1;
            q_mutex->lock();
            q.push(*it);
            q_mutex->unlock();
        }
    }

    for (auto it = graph.neighbours[currentVertex].rbegin();
        it != graph.neighbours[currentVertex].rend(); it++)
    {
        if (locked && *it < currentVertex)
        {
            mutexes[currentVertex]->unlock();
            locked = false;
        }
        mutexes[*it]->unlock();
    }

    if (locked)
        mutexes[currentVertex]->unlock();
}

void parallelBFSColouring(Graph& graph, int numberOfColours)
{
    boost::asio::thread_pool th_pool(8);

    std::mutex* q_mutex = new std::mutex();
    std::vector<std::mutex*> mutexes(graph.size);
        //Initialize mutexes
    for (int i = 0;i < graph.size;++i)
        mutexes[i] = new std::mutex();

    std::vector<int> visited(graph.size, 0);

    int maxColors = 1;

    visited[0] = 1;
    std::queue<int> q;
    q.push(0);

    bool validNumberOfColors = true;

    while (true)
    {
        if (!q.empty()) {

            q_mutex->lock();
            int top = q.front();
            q.pop();
            q_mutex->unlock();

            boost::asio::post(th_pool, [&graph, top, &visited, &maxColors, numberOfColours, &q, mutexes, q_mutex, &validNumberOfColors]() mutable
                {
                    threadFunction(graph, top, visited, maxColors, numberOfColours, q, mutexes, q_mutex, validNumberOfColors);
                });
        }

        q_mutex->lock();
        if (q.empty() && std::find(visited.begin(), visited.end(), 0) == visited.end()) {
            q_mutex->unlock();
            break;
        }
        q_mutex->unlock();

        if (!validNumberOfColors) {
            th_pool.join();
            for (int i = 0;i < graph.size;++i)
                delete mutexes[i];
            delete q_mutex;
            throw ColouringException();
        }
    }
    th_pool.join();

    for (int i = 0;i < graph.size;++i) 
        delete mutexes[i];
    delete q_mutex;
}

void color(Graph& graph, int m)
{
    // Create a visited array of n
// nodes, initialized to zero
    std::vector<int> visited(graph.size, 0);

    // maxColors used till now are 1 as
    // all nodes are painted color 1
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
                if (maxColors > m)
                    return ;

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
    int size = 6;

    Graph graph = Graph(size);
    Graph graph2 = Graph(size);
    createGraph(graph);
    createGraph(graph2);

    color(graph2, 3);
    for (auto it = graph2.colors.begin(); it != graph2.colors.end(); ++it)
    {
        std::cout << *it << " ";
    }
    std::cout << "\n";
    try
    {
        parallelBFSColouring(graph, 3);
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
