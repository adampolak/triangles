#include "graph.h"

#include <cstdlib>
#include <iostream>
using namespace std;

int main(int argc, char *argv[]) {
  if (argc != 3) {
    cerr << "Usage: " << argv[0] << " IN OUT" << endl;
    exit(1);
  }

  Edges edges = ReadEdgesFromDIMACSFile(argv[1]);
  NormalizeEdges(&edges);
  WriteEdgesToFile(edges, argv[2]);
}
