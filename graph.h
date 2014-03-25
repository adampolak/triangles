#ifndef GRAPH_H
#define GRAPH_H

#include <vector>
#include <utility>

typedef std::vector< std::pair<int, int> > Edges;
typedef std::vector< std::vector<int> > AdjList;

int NumVertices(const Edges& edges);

/*
struct AdjArray {
  std::vector<int> per_node;
  std::vector<int> per_edge;
};
*/

Edges ReadEdgesFromSNAPFile(const char* filename);
Edges ReadEdgesFromFile(const char* filename);
void WriteEdgesToFile(const Edges& edges, const char* filename);

void RemoveDuplicateEdges(Edges* edges);
void RemoveSelfLoops(Edges* edges);
void MakeUndirected(Edges* edges);
void PermuteEdges(Edges* edges);
void PermuteVertices(Edges* edges);

inline void NormalizeEdges(Edges* edges) {
  MakeUndirected(edges);
  RemoveDuplicateEdges(edges);
  RemoveSelfLoops(edges);
  PermuteEdges(edges);
  PermuteVertices(edges);
}

AdjList EdgesToAdjList(const Edges& edges);
#endif
