#include "gpu.h"

#include "timer.h"

#include <cuda_runtime.h>
#include <thrust/device_ptr.h>
#include <thrust/reduce.h>
#include <thrust/sort.h>

#include <algorithm>
#include <cassert>
#include <cstdlib>
#include <iostream>
#include <vector>
#include <utility>
using namespace std;

struct IsBackward {
  __host__ __device__ bool operator()(const unsigned long long edge) {
    int a = edge >> 32;
    int b = (edge << 32) >> 32;
    return a < b;
  }
};

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

__global__ void CalculateTriangles(int m, int* edges, int* pointers, int* results) {
  int from = blockDim.x * blockIdx.x + threadIdx.x;
  int step = gridDim.x * blockDim.x;
  for (int i = from; i < m; i += step) {
    int u = edges[2 * i], v = edges[2 * i + 1];
    int u_start = pointers[u], u_end = pointers[u + 1];
    int v_start = pointers[v], v_end = pointers[v + 1];
    int u_it = u_start, v_it = v_start;
    int count = 0;
    while (u_it < u_end && v_it < v_end) {
      int a = edges[2 * u_it], b = edges[2 * v_it];
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

  Log() << "Calc num vertices " << timer->SinceLast();
  
  int* dev_edges;
  int* dev_pointers;
  int* dev_results;

  CUCHECK(cudaMalloc(&dev_edges, m * 2 * sizeof(int)));
  CUCHECK(cudaMalloc(&dev_pointers, (n + 1) * sizeof(int)));
  CUCHECK(cudaMalloc(&dev_results, m * sizeof(int)));

  Log() << "cudaMalloc " << timer->SinceLast();


  CUCHECK(cudaMemcpyAsync(dev_edges, unordered_edges.data(),
                          m * 2 * sizeof(int),
                          cudaMemcpyHostToDevice));

  CUCHECK(cudaDeviceSynchronize());

  Log() << "Memcpy done " << timer->SinceLast();

  thrust::remove_if(
     thrust::device_ptr<uint64_t>((uint64_t*)dev_edges),
     thrust::device_ptr<uint64_t>((uint64_t*)dev_edges + m),
     IsBackward());

  m /= 2;

  thrust::sort(
    thrust::device_ptr<uint64_t>((uint64_t*)dev_edges),
    thrust::device_ptr<uint64_t>((uint64_t*)dev_edges + m));
  
  CUCHECK(cudaDeviceSynchronize());

  Log() << "Sort done " << timer->SinceLast();

  CalculatePointers<<<48, 256>>>(n, m, dev_edges, dev_pointers);

  CUCHECK(cudaDeviceSynchronize());

  Log() << "Calc ptrs kernel done " << timer->SinceLast();

  CalculateTriangles<<<48, 256>>>(m, dev_edges, dev_pointers, dev_results);

  CUCHECK(cudaDeviceSynchronize());

  Log() << "Calc tri kernel done " << timer->SinceLast();

  uint64_t result = 0;
  result = thrust::reduce(
      thrust::device_ptr<int>(dev_results),
      thrust::device_ptr<int>(dev_results + m));

  Log() << "Reduce done " << timer->SinceLast();

  CUCHECK(cudaFree(dev_edges));
  CUCHECK(cudaFree(dev_pointers));
  CUCHECK(cudaFree(dev_results));
  
  Log() << "Free done " << timer->SinceLast();

  delete timer;

  return result;
}
