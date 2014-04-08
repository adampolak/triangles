#include "gpu.h"

#include "gpu-thrust.h"
#include "timer.h"

#include <cuda_profiler_api.h>
#include <cuda_runtime.h>

#include <algorithm>
#include <cassert>
#include <cstdlib>
#include <iostream>
#include <vector>
#include <utility>
using namespace std;

#define NUM_BLOCKS 42
#define NUM_THREADS 128
#define WARP_SIZE 4

__global__ void CalculatePointers(int n, int m, int* edges, int* pointers) {
  int from = blockDim.x * blockIdx.x + threadIdx.x;
  int step = gridDim.x * blockDim.x;
  for (int i = from; i < m; i += step) {
    int prev = i > 0 ? edges[2 * (i - 1) + 1] : -1;
    int next = edges[2 * i + 1];
    for (int j = prev + 1; j <= next; ++j)
      pointers[j] = i;
  }
  if (from == 0) pointers[n] = m;
}

__global__ void CalculateFlags(int m, int* edges, int* pointers, int* flags) {
  int from = blockDim.x * blockIdx.x + threadIdx.x;
  int step = gridDim.x * blockDim.x;
  for (int i = from; i < m; i += step) {
    int a = edges[2 * i];
    int b = edges[2 * i + 1];
    int deg_a = pointers[a + 1] - pointers[a];
    int deg_b = pointers[b + 1] - pointers[b];
    flags[i] = (deg_a < deg_b) || (deg_a == deg_b && a < b);
  }
}

__global__ void UnzipEdges(int m, int* edges, int* unzipped_edges) {
  int from = blockDim.x * blockIdx.x + threadIdx.x;
  int step = gridDim.x * blockDim.x;
  for (int i = from; i < m; i += step) {
    unzipped_edges[i] = edges[2 * i];
    unzipped_edges[m + i] = edges[2 * i + 1];
  }
}

__global__ void CalculateTriangles(
    int m, int* edges, int* pointers, int* results) {
  int from = (NUM_THREADS * blockIdx.x + threadIdx.x) / WARP_SIZE;
  int step = NUM_BLOCKS * NUM_THREADS / WARP_SIZE;
  for (int i = from; i < m; i += step) {
    int u = edges[i], v = edges[m + i];

    int u_it = pointers[u], u_end = pointers[u + 1];
    int v_it = pointers[v], v_end = pointers[v + 1];
    int count = 0;

    while (u_it < u_end && v_it < v_end) {
      int a = edges[u_it], b = edges[v_it];
      if (a < b) {
        ++u_it;
      } else if (a > b) {
        ++v_it;
      } else {
        ++count;
        ++u_it;
        ++v_it;
      }
    }
    results[i] = count;
  }
}

void CudaAssert(cudaError_t status, const char* code, const char* file, int l) {
  if (status == cudaSuccess) return;
  cerr << "Cuda error: " << code << ", file " << file << ", line " << l << endl;
  exit(1);
}

#define CUCHECK(x) CudaAssert(x, #x, __FILE__, __LINE__)

uint64_t GpuEdgeIterator(const Edges& unordered_edges) {
  Timer* timer = Timer::NewTimer();
  
  int n = NumVertices(unordered_edges);
  int m = unordered_edges.size();

  Log() << "Calculate num vertices " << timer->SinceLast();

  int* dev_edges;
  int* dev_edges_unzipped;
  int* dev_pointers;
  int* dev_results;

  CUCHECK(cudaMalloc(&dev_edges, m * 2 * sizeof(int)));
  CUCHECK(cudaMalloc(&dev_edges_unzipped, m * 2 * sizeof(int)));
  CUCHECK(cudaMalloc(&dev_pointers, (n + 1) * sizeof(int)));
  CUCHECK(cudaMalloc(&dev_results, m * sizeof(int)));
  Log() << "Malloc " << timer->SinceLast();


  CUCHECK(cudaMemcpyAsync(dev_edges, unordered_edges.data(),
                          m * 2 * sizeof(int),
                          cudaMemcpyHostToDevice));
  CUCHECK(cudaDeviceSynchronize());
  Log() << "Memcpy " << timer->SinceLast();

  SortEdges(m, dev_edges);
  CUCHECK(cudaDeviceSynchronize());
  Log() << "Sort edges " << timer->SinceLast();

  CalculatePointers<<<NUM_BLOCKS, NUM_THREADS>>>(n, m, dev_edges, dev_pointers);
  CUCHECK(cudaDeviceSynchronize());
  Log() << "Calculate ptrs 1 " << timer->SinceLast();

  CalculateFlags<<<NUM_BLOCKS, NUM_THREADS>>>(
      m, dev_edges, dev_pointers, dev_results);
  RemoveMarkedEdges(m, dev_edges, dev_results);
  CUCHECK(cudaDeviceSynchronize());
  Log() << "Remove backward edges " << timer->SinceLast();

  m /= 2;
 
  CalculatePointers<<<NUM_BLOCKS, NUM_THREADS>>>(n, m, dev_edges, dev_pointers);
  CUCHECK(cudaDeviceSynchronize());
  Log() << "Calculate ptrs 2 " << timer->SinceLast();

  UnzipEdges<<<NUM_BLOCKS, NUM_THREADS>>>(m, dev_edges, dev_edges_unzipped);
  CUCHECK(cudaDeviceSynchronize());
  Log() << "Unzip edges " << timer->SinceLast();

  cudaProfilerStart();
  CalculateTriangles<<<NUM_BLOCKS, NUM_THREADS>>>(
      m, dev_edges_unzipped, dev_pointers, dev_results);
  CUCHECK(cudaDeviceSynchronize());
  cudaProfilerStop();
  Log() << "Calculate triangles " << timer->SinceLast();

  uint64_t result = 0;
  result = SumResults(m, dev_results);
  Log() << "Reduce " << timer->SinceLast();

  CUCHECK(cudaFree(dev_edges));
  CUCHECK(cudaFree(dev_edges_unzipped));
  CUCHECK(cudaFree(dev_pointers));
  CUCHECK(cudaFree(dev_results));
  Log() << "Free " << timer->SinceLast();

  delete timer;

  return result;
}
