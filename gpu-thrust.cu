#include "gpu-thrust.h"

#include <thrust/device_ptr.h>
#include <thrust/reduce.h>
#include <thrust/sort.h>

struct IsBackward {
  int* pointers;

  IsBackward(int* pointers) : pointers(pointers) {}
  __device__ bool operator()(uint64_t edge) {
    int a = edge >> 32;
    int b = (edge << 32) >> 32;
    int deg_a = pointers[a + 1] - pointers[a];
    int deg_b = pointers[b + 1] - pointers[b];
    return (deg_a > deg_b) || (deg_a == deg_b && a < b);
  }
};

void SortEdges(int m, int* edges) {
  thrust::device_ptr<uint64_t> ptr((uint64_t*)edges);
  thrust::sort(ptr, ptr + m);
}

void RemoveBackwardEdges(int m, int *edges, int* pointers) {
  thrust::device_ptr<uint64_t> ptr((uint64_t*)edges);
  thrust::remove_if(ptr, ptr + m, IsBackward(pointers));
}

uint64_t SumResults(int m, int* results) {
  thrust::device_ptr<int> ptr(results);
  return thrust::reduce(ptr, ptr + m);
}
