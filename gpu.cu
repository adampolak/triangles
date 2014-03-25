#include "gpu.h"

#include <cuda_runtime.h>
#include <thrust/device_ptr.h>
#include <thrust/sort.h>

#include <algorithm>
#include <cassert>
#include <cstdio>
#include <iostream>
#include <vector>
#include <utility>
using namespace std;

__global__ void CalculatePointers(int n, int m, int* edges, int* pointers) {
  int from = blockDim.x * blockIdx.x + threadIdx.x;
  int step = gridDim.x * blockDim.x;
  for (int i = from; i < m; i += step) {
    int prev = i > 0 ? edges[2 * (i - 1)] : -1;
    int next = edges[2 * i];
    for (int j = prev + 1; j <= next; ++j)
      pointers[j] = i;
  }
  if (from == 0) pointers[n] = m;
}

void CudaAssert(cudaError_t status, const char* code, const char* file, int l) {
  if (status == cudaSuccess) return;
  cerr << "Cuda error: " << code << ", file " << file << ", line " << l << endl;
  exit(1);
}

#define CUCHECK(x) CudaAssert(x, #x, __FILE__, __LINE__)

uint64_t GpuEdgeIterator(const Edges& unordered_edges) {
  //Timer timer;
  
  const int n = NumVertices(unordered_edges);
  const int m = unordered_edges.size();

  //Log() << "Calc num vertices " << timer.SinceLast();
  
  //Log() << "Device initialization " << timer.SinceLast();

  int* dev_edges;
  int* dev_pointers;
  int* dev_results;

  CUCHECK(cudaMalloc(&dev_edges, m * 2 * sizeof(int)));
  CUCHECK(cudaMalloc(&dev_pointers, (n + 1) * sizeof(int)));
  CUCHECK(cudaMalloc(&dev_results, m * sizeof(int)));

  //Log() << "cudaMalloc " << timer.SinceLast();

  cerr << "Malloc done" << endl;

  CUCHECK(cudaMemcpyAsync(dev_edges, unordered_edges.data(),
                          m * 2 * sizeof(int),
                          cudaMemcpyHostToDevice));

  cerr << "Memcpy done" << endl;

  
  thrust::sort(thrust::device_ptr<uint64_t>((uint64_t*)dev_edges),
               thrust::device_ptr<uint64_t>((uint64_t*)dev_edges + m));
  

  cerr << "Sort done" << endl;

  CalculatePointers<<<(m + 256 - 1) / 256, 256>>>(n, m, dev_edges, dev_pointers);

  CUCHECK(cudaDeviceSynchronize());

  cerr << "Calc ptrs kernel done" << endl;



  CUCHECK(cudaFree(dev_edges));
  CUCHECK(cudaFree(dev_pointers));
  CUCHECK(cudaFree(dev_results));
  
  cerr << "Free done" << endl;

  return 0;
}

