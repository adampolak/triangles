#include "forward.h"
#include "gpu.h"
#include "graph.h"
#include "timer.h"

#include <cassert>
#include <cstdlib>
#include <iostream>
#include <memory>
#include <vector>
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

  vector<uint64_t> results;
  t->SinceLast();  // reset t
  //results.push_back(Forward(graph));
  Log() << "Forward:             " << t->SinceLast();
  //results.push_back(CompactForward(graph));
  Log() << "Compact forward:     " << t->SinceLast();
  results.push_back(CompactForwardWithPreproc(edges));
  Log() << "Alternative preproc: " << t->SinceLast();
  results.push_back(GpuEdgeIterator(edges));
  Log() << "GPU edge iterator: " << t->SinceLast();
  for (uint64_t result : results)
    cerr << result << " ";
  cerr << endl;
  if (results.size() > 0)
    for (uint64_t result : results)
      assert(result == results.front());
}
