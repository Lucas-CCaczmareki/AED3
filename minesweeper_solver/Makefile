ifeq ($(OS),Windows_NT)
    EXT := .exe
    GENERATOR := -G "MinGW Makefiles"
else
    EXT :=
    GENERATOR :=
endif
 
.PHONY: build run clean

build:
	mkdir -p build
	cd build && cmake .. $(GENERATOR) && cmake --build .
 
run: build
	cd build && ./benchmark$(EXT) && ./tests$(EXT)
 
clean:
	rm -rf build
 