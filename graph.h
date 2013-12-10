#ifndef GRAPH_H
#define GRAPH_H

#include <vector>
#include <utility>

typedef std::vector<std::pair<int, int>> Edges;
typedef std::vector<std::vector<int>> AdjList;

/*
struct AdjArray {
  std::vector<int> per_node;
  std::vector<int> per_edge;
};
*/

Edges ReadEdgesFromFile(const char* filename);

AdjList EdgesToAdjList(const Edges& edges);

void Permute(Edges* edges);

#endif
