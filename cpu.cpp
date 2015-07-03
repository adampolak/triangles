#include "cpu.h"
#include "timer.h"

#include <algorithm>
#include <cassert>
#include <memory>
#include <vector>
#include <utility>
using namespace std;

namespace {
template <class InputIterator1, class InputIterator2>
int IntersectionSize(
    InputIterator1 first1, InputIterator1 last1,
    InputIterator2 first2, InputIterator2 last2) {
  int result = 0;
  while (first1 != last1 && first2 != last2) {
    if (*first1 < *first2)
      ++first1;
    else if (*first1 > *first2)
      ++first2;
    else {
      ++result;
      ++first1;
      ++first2;
    }
  }
  return result;
}
}  // namespace

uint64_t CpuForward(const AdjList& graph) {
  const int n = graph.size();
  vector<vector<int>> A(n);
  
  vector<pair<int, int>> deg(n);
  for (int i = 0; i < n; ++i)
    deg[i] = make_pair(graph[i].size(), i);
  sort(deg.begin(), deg.end(), greater<pair<int, int>>());
  vector<int> perm(n);
  for (int i = 0; i < n; ++i)
    perm[deg[i].second] = i;

  uint64_t c = 0;
  for (int i = 0; i < n; ++i) {
    const int s = deg[i].second;
    for (const int t : graph[s]) {
      if (perm[t] <= i) continue;
      c += IntersectionSize(A[s].begin(), A[s].end(), A[t].begin(), A[t].end());
      A[t].push_back(i);
    }
  }

  return c;
}

uint64_t CpuCompactForward(const AdjList& graph) {
  const int n = graph.size();

  vector<pair<int, int>> deg;
  deg.reserve(n);
  for (int i = 0; i < n; ++i)
    deg.push_back(make_pair(graph[i].size(), i));
  sort(deg.begin(), deg.end(), greater<pair<int, int>>());
  vector<int> perm(n);
  for (int i = 0; i < n; ++i)
    perm[deg[i].second] = i;
 
  AdjList sorted_graph(n);
  for (int i = 0; i < n; ++i) {
    const vector<int>& src = graph[i];
    const int s = perm[i];
    vector<int>& dst = sorted_graph[s];
    dst.reserve(src.size());  // this wastes some memory but saves a lot of time
    for (int t : src) {
      t = perm[t];
      if (t < s)
        dst.push_back(t);
    }
    sort(dst.begin(), dst.end());
  }

  uint64_t c = 0;
  for (int s = 0; s < n; ++s) {
    for (const int t : sorted_graph[s]) {
      c += IntersectionSize(
          sorted_graph[s].begin(), sorted_graph[s].end(),
          sorted_graph[t].begin(), sorted_graph[t].end());
    }
  }

  return c;
}

uint64_t CpuCompactForwardForEdgeArray(const Edges& edges) {
  unique_ptr<Timer> timer(Timer::NewTimer());
    
  const int n = NumVertices(edges);
  timer->Done("Calculate number of vertices");
  
  Edges sorted_edges;
  {
    vector<int> deg(n);
    for (const pair<int, int>& edge : edges)
      deg[edge.first]++;
    timer->Done("Calculate degrees");
    vector<pair<int, int>> temp(n);
    for (int i = 0; i < n; ++i)
      temp[i] = make_pair(n - deg[i], i);
    sort(temp.begin(), temp.end());
    vector<int> rename(n);
    for (int i = 0; i < n; ++i)
      rename[temp[i].second] = i;
    timer->Done("Calculate renaming permutation");
    sorted_edges.reserve(edges.size() / 2);
    for (const pair<int, int>& edge : edges) {
      int s = edge.first, t = edge.second;
      s = rename[s];
      t = rename[t];
      if (s > t)
        sorted_edges.push_back(make_pair(s, t));
    }
    timer->Done("Copy and rename edges");
    sort(sorted_edges.begin(), sorted_edges.end());
    timer->Done("Sort edges");
  }

  vector<int> heads(sorted_edges.size());
  for (int i = 0; i < sorted_edges.size(); ++i)
    heads[i] = sorted_edges[i].second;

  vector<vector<int>::iterator> nodes;
  nodes.reserve(n + 1);
  for (int k = 0; k <= sorted_edges.front().first; ++k)
    nodes.push_back(heads.begin());
  for (int i = 1; i < sorted_edges.size(); ++i)
    if (sorted_edges[i-1].first != sorted_edges[i].first) {
      int k = sorted_edges[i].first - sorted_edges[i-1].first;
      while (k--)
        nodes.push_back(heads.begin() + i);
    }
  while (nodes.size() < n + 1)
    nodes.push_back(heads.end());

  timer->Done("Copy edges and calculate nodes");
  uint64_t c = 0;
  for (const pair<int, int>& edge : sorted_edges) {
    const int s = edge.first, t = edge.second;
    c += IntersectionSize(nodes[s], nodes[s+1], nodes[t], nodes[t+1]);
  }
  return c;
}
