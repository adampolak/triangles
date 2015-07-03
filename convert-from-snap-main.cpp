#include "graph.h"

#include <cassert>
#include <cstdio>
#include <cstdlib>
#include <fstream>
#include <iostream>
#include <string>
#include <sstream>
#include <unordered_map>
using namespace std;

Edges ReadEdgesFromSNAPFile(const char* filename) {
  Edges edges;
  unordered_map<int, int> nodes;
  int next_node = 0;
  ifstream in(filename);
  assert(in.is_open());
  string buf;
  while (getline(in, buf)) {
    if (buf.empty() || buf[0] == '#')
      continue;  // ignore empty lines and comments
    int node_a, node_b;
    sscanf(buf.c_str(), "%d %d", &node_a, &node_b);
    if (!nodes.count(node_a))
      nodes[node_a] = next_node++;
    if (!nodes.count(node_b))
      nodes[node_b] = next_node++;
    edges.push_back(make_pair(nodes[node_a], nodes[node_b]));
  }
  return edges;
}

int main(int argc, char *argv[]) {
  if (argc != 3) {
    cerr << "Usage: " << argv[0] << " IN OUT" << endl;
    exit(1);
  }

  Edges edges = ReadEdgesFromSNAPFile(argv[1]);
  NormalizeEdges(&edges);
  WriteEdgesToFile(edges, argv[2]);
}
