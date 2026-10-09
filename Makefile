CXX = g++
CXXFLAGS = -Wall -pthread

TARGETS = SingleCar MultipleCars Capacity

all: $(TARGETS)

SingleCar: SingleCar.cpp
	$(CXX) $(CXXFLAGS) $< -o $@

MultipleCars: MultipleCars.cpp
	$(CXX) $(CXXFLAGS) $< -o $@

Capacity: Capacity.cpp
	$(CXX) $(CXXFLAGS) $< -o $@

clean:
	rm -f $(TARGETS) $(addsuffix .exe,$(TARGETS))
