CXX=g++
CXXFLAGS=-std=c++0x -O3

NVCC=/usr/local/cuda/bin/nvcc
NVCCFLAGS=-m64 -arch sm_20

convert-from-snap-main: convert-from-snap-main.cpp graph.cpp
	$(CXX) $(CXXFLAGS) $^ -o $@

convert-to-latapy-main: convert-to-latapy-main.cpp graph.cpp
	$(CXX) $(CXXFLAGS) $^ -o $@

latapy: latapy.c
	$(CXX) $(CXXFLAGS) $^ -o $@

gpu.o: gpu.cu
	$(NVCC) $(NVCCFLAGS) -c gpu.cu -o gpu.o

%.o: %.cpp
	$(CXX) $(CXXFLAGS) -c $< -o $@

main: forward.o gpu.o graph.o main.o timer.o
	$(NVCC) $^ -o $@

.PHONY: clean

clean:
	rm -f *.o
	rm -f convert-from-snap-main convert-to-latapy latapy main
