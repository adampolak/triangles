#include "forward.h"
#include "gpu.h"
#include "graph.h"
#include "timer.h"

#include <cassert>
#include <cstdlib>
#include <iostream>
#include <memory>
using namespace std;

int main(int argc, char *argv[]) {
  if (argc != 2) {
    cerr << "Usage: " << argv[0] << " FILE" << endl;
    exit(1);
  }
 
  unique_ptr<Timer> t(Timer::NewTimer());

  Edges edges = ReadEdgesFromFile(argv[1]);
  Log() << "Read file:           " << t->SinceLast();
  AdjList graph = EdgesToAdjList(edges);
  Log() << "Convert to adj list: " << t->SinceLast();

  int num_nodes = graph.size();
  int num_edges = 0;
  for (const auto& neighbors : graph)
    num_edges += neighbors.size();
  Log() << "Num nodes: " << num_nodes << ", num edges: " << num_edges;

  t->SinceLast();  // reset t
  /*
  uint64_t c1 = Forward(graph);
  Log() << "Forward:             " << t->SinceLast();
  uint64_t c2 = CompactForward(graph);
  Log() << "Compact forward:     " << t->SinceLast();
  uint64_t c3 = CompactForwardWithPreproc(edges);
  Log() << "Alternative preproc: " << t->SinceLast();
  */
  uint64_t c4 = GpuEdgeIterator(edges);
  Log() << "GPU edge iterator: " << t->SinceLast();
  /*
  Log() << c1 << " " << c2 << " " << c3;
  assert(c1 == c2 && c2 == c3);
  */
}
