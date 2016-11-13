SHELL := /bin/bash
CC := c++
CFLAGS := -std=c++0x -O3
TESTS := $(shell ls -d test/*cpp | sed -e "s/test\///g" | sed -e "s/.cpp//g")

.PHONY: build run test clean $(TESTS)

all: build

build: bin
	$(CC) $(CFLAGS) -o bin/exacto src/main.cpp

run: build
	bin/exacto

clean:
	rm -rf bin
	rm -rf test/bin

bin:
	mkdir -p bin

test: clean build $(TESTS)
	@printf "\nOK\n\n"

$(TESTS):
	mkdir -p test/bin
	@printf "\n=== Building tests: "$@" ===\n"
	$(CC) $(CFLAGS) -o test/bin/$@ test/$@.cpp
	@printf "\n=== Running tests:  "$@" ===\n"
	@test/bin/$@
	@rm -rf test/bin

