#include "cpu.h"
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
  // t->Done("Convert to adjacency lists");

  int num_nodes = NumVertices(edges);
  int num_edges = edges.size();
  cerr << "Number of nodes: " << num_nodes
       << ", number of edges: " << num_edges << endl;
  
  vector<uint64_t> results;

#define TEST_ALGORITHM(algorithm, ...) do { \
  results.push_back(algorithm(__VA_ARGS__)); \
  t->Done("ALGORITHM: "#algorithm); \
} while (false);

  t->Reset();
  // TEST_ALGORITHM(CpuForward, graph);
  // TEST_ALGORITHM(CpuCompactForward, graph);
  // TEST_ALGORITHM(CpuCompactForwardForEdgeArray, edges);
  PreInitGpuContext(0);
  t->Done("Preinitialize context for device 0");
  TEST_ALGORITHM(GpuForward, edges);
  // for (int i = 0; i < 4; ++i)
  //   PreInitGpuContext(i);
  // t->Done("Preinitialize context for all four devices");
  // TEST_ALGORITHM(MultiGpuForward, edges, 4);

  for (uint64_t result : results)
    cerr << result << " ";
  cerr << endl;
  if (results.size() > 0)
    for (uint64_t result : results)
      assert(result == results.front());
}
