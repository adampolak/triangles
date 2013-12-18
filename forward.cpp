#include "forward.h"
#include "timer.h"

#include <algorithm>
#include <cassert>
#include <vector>
#include <utility>
using namespace std;

namespace {
template <class InputIterator1, class InputIterator2>
int IntersectionSize(
    InputIterator1 first1, InputIterator1 last1,
    InputIterator2 first2, InputIterator2 last2,
    int skip_ge) {
  int result = 0;
  while (first1 != last1 && first2 != last2) {
    if (*first1 >= skip_ge || *first2 >= skip_ge)
      break;
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

uint64_t Forward(const AdjList& graph) {
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
      c += IntersectionSize(
          A[s].begin(), A[s].end(), A[t].begin(), A[t].end(), n);
      A[t].push_back(i);
    }
  }

  return c;
}

uint64_t CompactForward(const AdjList& graph) {
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
    sorted_graph[perm[i]] = graph[i];
  }
  for (auto& neighbors : sorted_graph) {
    for (int& neighbor : neighbors)
      neighbor = perm[neighbor];
    sort(neighbors.begin(), neighbors.end());
  }

  uint64_t c = 0;
  for (int s = 0; s < n; ++s) {
    for (const int t : sorted_graph[s]) {
      if (t <= s) continue;
      c += IntersectionSize(
          sorted_graph[s].begin(), sorted_graph[s].end(),
          sorted_graph[t].begin(), sorted_graph[t].end(),
          s);
    }
  }

  return c;
}

uint64_t CompactForwardWithPreproc(const Edges& unordered_edges) {
  const int n = NumVertices(unordered_edges);
  Edges edges(unordered_edges.size());
  {
    vector<int> deg(n);
    for (const pair<int, int>& edge : unordered_edges)
      deg[edge.first]++;
    vector<pair<int, int>> temp(n);
    for (int i = 0; i < n; ++i)
      temp[i] = make_pair(n - deg[i], i);
    sort(temp.begin(), temp.end());
    vector<int> rename(n);
    for (int i = 0; i < n; ++i)
      rename[temp[i].second] = i;
    for (int i = 0; i < unordered_edges.size(); ++i) {
      edges[i].first = rename[unordered_edges[i].first];
      edges[i].second = rename[unordered_edges[i].second];
    }
    sort(edges.begin(), edges.end());
  }

  Timer timer;

  vector<int> ends(edges.size());
  for (int i = 0; i < edges.size(); ++i)
    ends[i] = edges[i].second;

  Log() << "Compacting edges " << timer.SinceStart();

  vector<vector<int>::iterator> pointers;
  pointers.reserve(n + 1);
  pointers.push_back(ends.begin());
  for (int i = 1; i < edges.size(); ++i)
    if (edges[i-1].first != edges[i].first) {
      assert(edges[i-1].first + 1 == edges[i].first);  // no isolated vertices
      pointers.push_back(ends.begin() + i);
    }
  pointers.push_back(ends.end());

  Log() << "Storing pointers " << timer.SinceStart();

  uint64_t c = 0;
  for (const pair<int, int>& edge : edges) {
    const int s = edge.first, t = edge.second;
    if (t <= s) continue;
    c += IntersectionSize(
        pointers[s], pointers[s+1], pointers[t], pointers[t+1], s);
  }
  return c;
}
