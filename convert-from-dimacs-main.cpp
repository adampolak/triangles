#include "graph.h"

#include <cassert>
#include <cstdio>
#include <cstdlib>
#include <fstream>
#include <iostream>
#include <string>
#include <sstream>
using namespace std;

Edges ReadEdgesFromDIMACSFile(const char* filename) {
  Edges edges;
  ifstream in(filename);
  assert(in.is_open());
  string buf;
  getline(in, buf);
  int n, m;
  sscanf(buf.c_str(), "%d %d", &n, &m);
  edges.reserve(2 * m);
  for (int i = 0; i < n; ++i) {
    getline(in, buf);
    istringstream parser(buf);
    int neighbor;
    while (parser >> neighbor) {
      edges.push_back(make_pair(i, neighbor - 1));
    }
  }
  return edges;
}

int main(int argc, char *argv[]) {
  if (argc != 3) {
    cerr << "Usage: " << argv[0] << " IN OUT" << endl;
    exit(1);
  }

  Edges edges = ReadEdgesFromDIMACSFile(argv[1]);
  NormalizeEdges(&edges);
  WriteEdgesToFile(edges, argv[2]);
}
