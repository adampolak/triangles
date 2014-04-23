#include "gpu-thrust.h"

#include <thrust/device_ptr.h>
#include <thrust/functional.h>
#include <thrust/reduce.h>
#include <thrust/sort.h>

int NumVerticesGPU(int m, int* edges) {
  thrust::device_ptr<int> ptr(edges);
  return 1 + thrust::reduce(ptr, ptr + 2 * m, 0, thrust::maximum<int>());
}

void SortEdges(int m, int* edges) {
  thrust::device_ptr<uint64_t> ptr((uint64_t*)edges);
  thrust::sort(ptr, ptr + m);
}

void RemoveMarkedEdges(int m, int* edges, bool* flags) {
  thrust::device_ptr<uint64_t> ptr((uint64_t*)edges);
  thrust::device_ptr<bool> ptr_flags(flags);
  thrust::remove_if(ptr, ptr + m, ptr_flags, thrust::identity<bool>());
}

uint64_t SumResults(int size, uint64_t* results) {
  thrust::device_ptr<uint64_t> ptr(results);
  return thrust::reduce(ptr, ptr + size);
}
