#include "graph.h"

#include <cassert>
#include <cstdlib>
#include <iostream>
#include <set>
#include <vector>
using namespace std;

int main(int argc, char *argv[]) {
  if (argc != 5) {
    cerr << "Usage: " << argv[0] << " N K P OUT" << endl;
    exit(1);
  }

  int n = atoi(argv[1]);
  int k = atoi(argv[2]);
  int p = atoi(argv[3]);

  Edges edges;
  for (int i = 0; i < n; ++i) {
    for (int j = 0; j < k / 2; ++j)  {
      int neighbor = (i + j + 1) % n;
      if (rand() % 100 < p)
        neighbor = rand() % n;
      edges.push_back(make_pair(i, neighbor));
    }
 }

  NormalizeEdges(&edges);

  WriteEdgesToFile(edges, argv[4]);
}
