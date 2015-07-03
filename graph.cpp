#include "graph.h"

#include <algorithm>
#include <fstream>
using namespace std;

Edges ReadEdgesFromFile(const char* filename) {
  Edges edges;
  ifstream in(filename, ios::binary);
  int m;
  in.read((char*)&m, sizeof(int));
  edges.resize(m);
  in.read((char*)edges.data(), 2 * m * sizeof(int));
  return edges;
}

void WriteEdgesToFile(const Edges& edges, const char* filename) {
  ofstream out(filename, ios::binary);
  int m = edges.size();
  out.write((char*)&m, sizeof(int));
  out.write((char*)edges.data(), 2 * m * sizeof(int));
}

int NumVertices(const Edges& edges) {
  int num_vertices = 0;
  for (const pair<int, int>& edge : edges)
    num_vertices = max(num_vertices, 1 + max(edge.first, edge.second));
  return num_vertices;
}

void RemoveDuplicateEdges(Edges* edges) {
  sort(edges->begin(), edges->end());
  edges->erase(unique(edges->begin(), edges->end()), edges->end());
}

void RemoveSelfLoops(Edges* edges) {
  for (int i = 0; i < edges->size(); ++i) {
    if ((*edges)[i].first == (*edges)[i].second) {
      edges->at(i) = edges->back();
      edges->pop_back();
      --i;
    }
  }
}

void MakeUndirected(Edges* edges) {
  const size_t n = edges->size();
  for (int i = 0; i < n; ++i) {
    pair<int, int> edge = (*edges)[i];
    swap(edge.first, edge.second);
    edges->push_back(edge);
  }  
}

void PermuteEdges(Edges* edges) {
  random_shuffle(edges->begin(), edges->end());
}

void PermuteVertices(Edges* edges) {
  vector<int> p(NumVertices(*edges));
  for (int i = 0; i < p.size(); ++i)
    p[i] = i;
  random_shuffle(p.begin(), p.end());
  for (pair<int, int>& edge : *edges) {
    edge.first = p[edge.first];
    edge.second = p[edge.second];
  }
}

AdjList EdgesToAdjList(const Edges& edges) {
  // Sorting edges with std::sort to optimize memory access pattern when
  // creating graph gives less than 20% speedup.
  AdjList graph(NumVertices(edges));
  for (const pair<int, int>& edge : edges)
    graph[edge.first].push_back(edge.second);
  return graph;
}
