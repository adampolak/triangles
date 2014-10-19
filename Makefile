CXX=g++
CXXFLAGS=-std=c++0x -O3 -fno-stack-protector

NVCC=/usr/local/cuda/bin/nvcc
NVCCFLAGS=-m64 -arch sm_20 -O3

convert-from-dimacs-main.e: convert-from-dimacs-main.cpp graph.cpp
	$(CXX) $(CXXFLAGS) $^ -o $@

convert-from-snap-main.e: convert-from-snap-main.cpp graph.cpp
	$(CXX) $(CXXFLAGS) $^ -o $@

convert-to-latapy-main.e: convert-to-latapy-main.cpp graph.cpp
	$(CXX) $(CXXFLAGS) $^ -o $@

latapy.e: latapy.c
	$(CXX) $(CXXFLAGS) $^ -o $@

create-barabasi-albert-main.e: create-barabasi-albert-main.cpp graph.cpp
	$(CXX) $(CXXFLAGS) $^ -o $@

create-watts-strogatz-main.e: create-watts-strogatz-main.cpp graph.cpp
	$(CXX) $(CXXFLAGS) $^ -o $@

%.o: %.cu %.h
	$(NVCC) $(NVCCFLAGS) -c $< -o $@

%.o: %.cpp %.h
	$(CXX) $(CXXFLAGS) -c $< -o $@

main.e: forward.o gpu.o gpu-thrust.o graph.o main.o timer.o
	$(NVCC) $^ -o $@

.PHONY: clean

clean:
	rm -f *.o *.e
