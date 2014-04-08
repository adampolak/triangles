CXX=g++
CXXFLAGS=-std=c++0x -O3

NVCC=/usr/local/cuda/bin/nvcc
NVCCFLAGS=-m64 -arch sm_20

convert-from-snap-main.e: convert-from-snap-main.cpp graph.cpp
	$(CXX) $(CXXFLAGS) $^ -o $@

convert-to-latapy-main.e: convert-to-latapy-main.cpp graph.cpp
	$(CXX) $(CXXFLAGS) $^ -o $@

latapy.e: latapy.c
	$(CXX) $(CXXFLAGS) $^ -o $@

gpu.o: gpu.cu
	$(NVCC) $(NVCCFLAGS) -c gpu.cu -o gpu.o

gpu-thrust.o: gpu-thrust.cu
	$(NVCC) $(NVCCFLAGS) -c gpu-thrust.cu -o gpu-thrust.o

%.o: %.cpp
	$(CXX) $(CXXFLAGS) -c $< -o $@

main.e: forward.o gpu.o gpu-thrust.o graph.o main.o timer.o
	$(NVCC) $^ -o $@

.PHONY: clean

clean:
	rm -f *.o *.e
