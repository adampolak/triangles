#ifndef GPU_THRUST_H
#define GPU_THRUST_H

#include <stdint.h>

int NumVerticesGPU(int m, int* edges);
void SortEdges(int m, int* edges);
void RemoveMarkedEdges(int m, int *edges, bool* flags);
uint64_t SumResults(int size, uint64_t* results);

#endif
