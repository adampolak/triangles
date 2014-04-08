#include "gpu-thrust.h"

#include <thrust/device_ptr.h>
#include <thrust/reduce.h>
#include <thrust/sort.h>

void SortEdges(int m, int* edges) {
  thrust::device_ptr<uint64_t> ptr((uint64_t*)edges);
  thrust::sort(ptr, ptr + m);
}

void RemoveMarkedEdges(int m, int* edges, int* flags) {
  thrust::device_ptr<uint64_t> ptr((uint64_t*)edges);
  thrust::device_ptr<int> ptr_flags(flags);
  thrust::remove_if(ptr, ptr + m, ptr_flags, thrust::identity<int>());
}

uint64_t SumResults(int m, int* results) {
  thrust::device_ptr<int> ptr(results);
  return thrust::reduce(ptr, ptr + m);
}
