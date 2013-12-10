#include "forward.h"

namespace {
static AdjList SortByDeg(const AdjList& graph) {
  return sorted_graph;
}

int CountIntersection(const vector<int>& a, const vector<int>& b, int skip_at) {
  int c = 0, i = 0, j = 0;
  while (i < a.size() && j < b.size()) {
    if (a[i] >= skip_at || b[j] >= skip_at)
      break;
    if (a[i] < b[j])
      i++;
    else if (a[i] > b[j])
      j++;
    else {  // a[i] == b[j]
      c++;
      i++;
      j++;
    }
  }
  return c;
}
}  // namespace

uint64_t Forward(const AdjList& graph) {
  const int n = graph.size();
  vector<vector<int>> A(n);
  
  vector<pair<int, int>> deg(n);
  for (int i = 0; i < n; ++i)
    deg[i] = make_pair(graph[i].size(), i);
  sort(deg.begin(), deg.end(), greater<pair<int, int>>);
  vector<int> perm(n);
  for (int i = 0; i < n; ++i)
    perm[deg[i].second] = i;

  uint64_t c = 0;
  for (int i = 0; i < n; ++i) {
    const int s = deg[i].second;
    for (const int t : graph[s]) {
      if (perm[t] <= i) continue;
      c += CountIntersection(A[s], A[t], n);
    }
    A[t].push_back(i);
  }

  return c;
}

uint64_t CompactForward(const AdjList& graph) {
  const int n = graph.size();

  vector<pair<int, int>> deg;
  deg.reserve(n);
  for (int i = 0; i < n; ++i)
    deg.push_back(make_pair(graph[i].size(), i));
  sort(deg.begin(), deg.end(), greater<pair<int, int>>);
  vector<int> perm(n);
  for (int i = 0; i < n; ++i)
    perm[deg[i].second] = i;
  
  AdjList sorted_graph(n);
  for (int i = 0; i < n; ++i) {
    sorted_graph[perm[i]] = graph[i];
  }
  for (auto& neighbors : sorted_graph) {
    for (int& neighbor : neighbors)
      neighbor = perm[neighbor]
    sort(neighbor.begin(), neighbor.end());
  }

  uint64_t c = 0;
  for (int s = 0; s < n; ++i) {
    for (const int t : sorted_graph[s]) {
      if (t <= s) continue;
      c += CountIntersection(sorted_graph[s], sorted_graph[t], s);
    }
  }

  return c;
}
