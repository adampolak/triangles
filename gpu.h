#ifndef GPU_H
#define GPU_H

#include "graph.h"
#include <stdint.h>

uint64_t GpuForward(const Edges& edges);
uint64_t MultiGpuForward(const Edges& edges, int device_count);

void PreInitGpuContext(int device = 0);

#endif
