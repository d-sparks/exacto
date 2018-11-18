SHELL := /bin/bash
CC := c++
CFLAGS := -std=c++11 -O3
SOURCE := $(shell ls -d src/*cpp | grep -v main) -lpthread
TESTS := $(shell ls -d test/*cpp | sed -e "s/test\///g" | sed -e "s/.cpp//g")
SUITES := $(shell ls -d data/*suite | sed -e "s/data\///g" | sed -e "s/.suite//g")

.PHONY: build run test clean debug suites $(TESTS) $(SUITES)

all: build

build: bin
	$(CC) $(CFLAGS) -o bin/exacto src/main.cpp $(SOURCE)

run: build
	bin/exacto

clean:
	rm -rf bin
	rm -rf util/bin
	rm -rf test/bin

debug: clean
	@mkdir -p util/bin
	$(CC) $(CFLAGS) -o util/bin/debug util/debug.cpp $(SOURCE)
	@util/bin/debug

run_suite: clean
	@mkdir -p util/bin
	$(CC) $(CFLAGS) -o util/bin/run_suite util/run_suite.cpp $(SOURCE)

suites: clean build $(SUITES)
	rm -rf util/bin

$(SUITES): run_suite
	@printf "\n=== Running suite: "$@" ===\n"
	util/bin/run_suite data/$@.suite > data/$@.suite_results

bin:
	mkdir -p bin

test: clean build $(TESTS)
	@printf "\nOK\n\n"

$(TESTS):
	mkdir -p test/bin
	@printf "\n=== Building tests: "$@" ===\n"
	$(CC) $(CFLAGS) -o test/bin/$@ test/$@.cpp $(SOURCE)
	@printf "\n=== Running tests:  "$@" ===\n"
	@test/bin/$@
	@rm -rf test/bin

