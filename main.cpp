#include "forward.h"
#include "graph.h"
#include "timer.h"

#include <cassert>
#include <cstdlib>
#include <iostream>
using namespace std;

int main(int argc, char *argv[]) {
  if (argc != 2) {
    cerr << "Usage: " << argv[0] << " FILE" << endl;
    exit(1);
  }
 
  Edges edges;
  AdjList graph;
  {
    Timer t;
    edges = ReadEdgesFromSNAPFile(argv[1]);
    Log() << "Reading file:        " << t.SinceLast();
    NormalizeEdges(&edges);
    Log() << "Normalize edges:     " << t.SinceLast();
    graph = EdgesToAdjList(edges);
    Log() << "Convert to adj list: " << t.SinceLast();
  }

  int num_nodes = graph.size();
  int num_edges = 0;
  for (const auto& neighbors : graph)
    num_edges += neighbors.size();
  Log() << "Num nodes: " << num_nodes << ", num edges: " << num_edges;

  Timer t;
  uint64_t c1 = Forward(graph);
  Log() << "Forward:             " << t.SinceLast();
  uint64_t c2 = CompactForward(graph);
  Log() << "Compact forward:     " << t.SinceLast();
  uint64_t c3 = CompactForwardWithPreproc(edges);
  Log() << "Alternative preproc: " << t.SinceLast();
  Log() << c1 << " " << c2 << " " << c3;
  assert(c1 == c2 && c2 == c3);
}
