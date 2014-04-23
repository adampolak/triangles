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
  t->Done("Read file");
  // AdjList graph = EdgesToAdjList(edges);
  t->Done("Convert to adjacency lists");

  int num_nodes = NumVertices(edges);
  int num_edges = edges.size();
  cerr << "Number of nodes: " << num_nodes
       << ", number of edges: " << num_edges << endl;
  
  vector<uint64_t> results;
  t->Reset();
  // results.push_back(Forward(graph));
  t->Done("ALGORITHM: Forward Classic");
  // results.push_back(CompactForward(graph));
  t->Done("ALGORITHM: Forward Compact");
  // results.push_back(CompactForwardWithPreproc(edges));
  t->Done("ALGORITHM: Alt preproc");
  results.push_back(GpuEdgeIterator(edges));
  t->Done("ALGORITHM: Forward GPU");
  // results.push_back(GpuEdgeIterator(edges, 4));
  t->Done("ALGORITHM: Forward multi GPU");
  for (uint64_t result : results)
    cerr << result << " ";
  cerr << endl;
  if (results.size() > 0)
    for (uint64_t result : results)
      assert(result == results.front());
}
