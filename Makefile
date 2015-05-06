SHELL := /bin/bash
CC := g++
CFLAGS := -std=c++0x
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

test/bin:
	mkdir -p test/bin

test: test/bin $(TESTS)
	@printf "\nOK\n\n"
	@rm -rf test/bin

$(TESTS): test/bin
	@printf "\n=== Building tests: "$@" ===\n"
	$(CC) $(CFLAGS) -o test/bin/$@ test/$@.cpp
	@printf "\n=== Running tests:  "$@" ===\n"
	@test/bin/$@

