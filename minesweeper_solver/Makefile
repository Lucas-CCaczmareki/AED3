ifeq ($(OS),Windows_NT)
    EXT := .exe
    GENERATOR := -G "MinGW Makefiles"
    RUN_PREFIX :=
    MKDIR := if not exist build mkdir build
    RMDIR := if exist build rmdir /s /q build
else
    EXT :=
    GENERATOR :=
    RUN_PREFIX := ./
    MKDIR := mkdir -p build
    RMDIR := rm -rf build
endif

.PHONY: build run clean

build:
	$(MKDIR)
	cd build && cmake .. $(GENERATOR) && cmake --build .

run: build
	cd build && $(RUN_PREFIX)autoplayer$(EXT)

clean:
	$(RMDIR)