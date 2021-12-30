#pragma once
#include <vector>
#include <set>

class Graph
{
public:
	unsigned int size;
	std::vector<std::set<int>> neighbours;
	std::vector<int> colors;
public:
	Graph(int size);

	void addEdge(int vertex1, int vertex2);

};

