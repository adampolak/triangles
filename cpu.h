#ifndef CPU_H
#define CPU_H

#include "graph.h"
#include <stdint.h>

uint64_t CpuForward(const AdjList& graph);
uint64_t CpuCompactForward(const AdjList& graph);
uint64_t CpuCompactForwardForEdgeArray(const Edges& edges);

#endif
