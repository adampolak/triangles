#include "graph.h"

#include <cassert>
#include <cstdlib>
#include <iostream>
#include <set>
#include <vector>
using namespace std;

int main(int argc, char *argv[]) {
  if (argc != 4) {
    cerr << "Usage: " << argv[0] << " M N OUT" << endl;
    exit(1);
  }

  int m = atoi(argv[1]);
  int n = atoi(argv[2]);

  assert(m >= 1 && m < n);

  Edges edges;
  for (int i = 0; i < m; ++i) {
    edges.push_back(make_pair(i, m));
    edges.push_back(make_pair(m, i));
  }

  for (int i = m + 1; i < n; ++i) {
    set<int> neighbors;
    while (neighbors.size() < m)
      neighbors.insert(edges[rand() % edges.size()].first);
    for (int neighbor: neighbors) {
      edges.push_back(make_pair(neighbor, i));
      edges.push_back(make_pair(i, neighbor));
    }
  }
  
  PermuteEdges(&edges);
  PermuteVertices(&edges);

  WriteEdgesToFile(edges, argv[3]);
}
