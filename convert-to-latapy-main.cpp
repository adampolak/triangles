#include "graph.h"

#include <cstdlib>
#include <fstream>
#include <iostream>
using namespace std;

bool HashAndChoose(unsigned x, unsigned y) {
  unsigned f = (x * x * x) ^ (1735915379 * y) ^ 874322;
  unsigned g = (y * y * y) ^ (1735915379 * x) ^ 874322;
  return (f < g) || (f == g && x < y);
}

int main(int argc, char *argv[]) {
  if (argc != 3) {
    cerr << "Usage: " << argv[0] << " IN OUT" << endl;
    exit(1);
  }

  Edges edges = ReadEdgesFromFile(argv[1]);
  int n = NumVertices(edges);
  vector<int> deg(n);
  for (const auto& edge : edges)
    ++deg[edge.first];

  ofstream out(argv[2]);
  out << n << endl;
  for (int i = 0; i < n; ++i)
    out << i << " " << deg[i] << endl;
  for (const auto& edge : edges)
    if (HashAndChoose(edge.first, edge.second))
      out << edge.first << " " << edge.second << endl;
}
