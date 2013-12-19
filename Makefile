CXXFLAGS=-std=c++0x -O3

NVCC=/usr/local/cuda/bin/nvcc
NVCCFLAGS=-Xcompiler="$(CXXFLAGS)" -m 64 -arch sm_20

main: forward.cpp graph.cpp main.cpp timer.cpp
	$(NVCC) $(NVCCFLAGS) $^ -o $@

convert-main: convert-main.cpp graph.cpp
	$(NVCC) $(NVCCFLAGS) $^ -o $@

.PHONY: clean

clean:
	rm -f main convert-main
