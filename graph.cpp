#include "graph.h"

#include <algorithm>
#include <cassert>
#include <fstream>
#include <regex>
#include <string>
#include <unordered_map>
using namespace std;

Edges ReadEdgesFromFile(const char* filename) {
  Edges edges;
  unordered_map<string, int> nodes;
  int next_node = 0;
  ifstream in(filename);
  assert(in.is_open());
  string buf;
  regex re_edge("[[:alnum:]]+[[:space:]][[:alnum:]]+",
                regex::extended);
  while (getline(in, buf)) {
    if (buf.length() > 0 && buf[0] == '#')
      continue;  // ignore comments
    smatch match;
    assert(regex_match(buf, match, re_edge));
    const string& node_a = match[1];
    const string& node_b = match[2];
    if (!nodes.count(node_a))
      nodes[node_a] = next_node++;
    if (!nodes.count(node_b))
      nodes[node_b] = next_node++;
    edges.push_back(make_pair(nodes[node_a], nodes[node_b]));
  }
  return edges;
}

namespace {
static int NumVertices(const Edges& edges) {
  int num_vertices = 0;
  for (pair<int, int> edge : edges)
    num_vertices = max(num_vertices, 1 + max(edge.first, edge.second));
  return num_vertices;
}
}  // namespace


AdjList EdgesToAdjList(const Edges& edges) {
  AdjList graph(NumVertices(edges));
  for (pair<int, int> edge : edges)
    graph[edge.first].push_back(edge.second);
}

void Permute(Edges* edges) {
  vector<int> p(NumVertices(*edges));
  for (int i = 0; i < p.size(); ++i)
    p[i] = i;
  random_shuffle(p.begin(), p.end());
  for (pair<int, int>& edge : *edges) {
    edge.first = p[edge.first];
    edge.second = p[edge.second];
  }
  random_shuffle(edges->begin(), edges->end());
}
