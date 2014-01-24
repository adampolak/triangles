CXX=g++
CXXFLAGS=-std=c++0x -O3

NVCC=/usr/local/cuda/bin/nvcc
NVCCFLAGS=-Xcompiler="$(CXXFLAGS)" -m 64 -arch sm_20

convert-from-snap-main: convert-from-snap-main.cpp graph.cpp
	$(CXX) $(CXXFLAGS) $^ -o $@

convert-to-latapy-main: convert-to-latapy-main.cpp graph.cpp
	$(CXX) $(CXXFLAGS) $^ -o $@

latapy: latapy.c
	$(CXX) $(CXXFLAGS) $^ -o $@

main: forward.cpp graph.cpp main.cpp timer.cpp
	$(NVCC) $(NVCCFLAGS) $^ -o $@


.PHONY: clean

clean:
	rm -f convert-from-snap-main convert-to-latapy latapy main
