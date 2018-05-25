
CXXFLAGS = -Wall -pedantic -std=c++11 -Wunused-function
TARGETS = test

.PHONY: test clean

all: test

test: test.cpp ../qprint.hpp
	clang++ $(CXXFLAGS) $< -o $@

clean:
	rm -rf test

