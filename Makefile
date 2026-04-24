# Convenience shim — delegates to build.sh
# Usage:
#   make          → release build
#   make debug    → debug build
#   make clean    → remove build/
#   make run      → build + run with data/

.PHONY: all debug clean run

all:
	@bash build.sh

debug:
	@cmake -S . -B build -DCMAKE_BUILD_TYPE=Debug
	@cmake --build build

clean:
	@rm -rf build/
	@echo "Cleaned build/"

run: all
	./build/strawhats --data-dir data
