#ifndef GPU_THRUST_H
#define GPU_THRUST_H

#include <stdint.h>

void SortEdges(int m, int* edges);
void RemoveBackwardEdges(int m, int *edges, int* pointers);
uint64_t SumResults(int m, int* results);

#endif
