#include "Graph.h"

Graph::Graph(int size)
{
	this->size = size;
	for (int i = 0;i < size;i++)
	{
		neighbours.push_back(std::set<int>());
		colors.push_back(-1);
	}
}

void Graph::addEdge(int vertex1, int vertex2)
{

	this->neighbours[vertex1].insert(vertex2);
	this->neighbours[vertex2].insert(vertex1);

}