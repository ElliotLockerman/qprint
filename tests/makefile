
TARGETS = test
CXXFLAGS = -Wall -std=c++11 -Wunused-function

.PHONY: all clean

all: $(TARGETS)

$(TARGETS): %: %.cpp ../qprint.hpp
	$(CXX) $(CXXFLAGS) $< -o $@

clean:
	@rm -f test 2>/dev/null || true

