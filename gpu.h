#ifndef GPU_H
#define GPU_H

#include "graph.h"
#include <stdint.h>

uint64_t GpuEdgeIterator(const Edges& unordered_edges);
uint64_t GpuEdgeIterator(const Edges& unordered_edges, int device_count);

void PreInitGpuContext(int device = 0);

#endif
