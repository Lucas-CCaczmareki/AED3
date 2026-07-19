ifeq ($(OS),Windows_NT)
    EXT := .exe
    GENERATOR := -G "MinGW Makefiles"
else
    EXT :=
    GENERATOR :=
endif

.PHONY: build run clean run-autoplayer run-k-convergence run-scaling run-sensitivity run-calibration run-all

build:
	mkdir -p build
	cd build && cmake .. $(GENERATOR) && cmake --build .

run: build
	cd build && ./benchmark$(EXT) && ./tests$(EXT)

run-autoplayer: build
	cd build && ./autoplayer$(EXT)

run-benchmark-mc-vs-bf: build
	cd build && ./benchmark_mc_vs_bf$(EXT)

run-all: build
	cd build && ./benchmark$(EXT) && ./tests$(EXT) && ./autoplayer$(EXT) && ./k_convergence$(EXT) && ./scaling_curve$(EXT) && ./sensitivity$(EXT) && ./calibration$(EXT)

clean:
	rm -rf build