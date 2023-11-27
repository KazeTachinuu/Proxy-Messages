# Makefile

CXX := g++
CXXFLAGS := -std=c++17 -Wall -Wextra -I./src
LDFLAGS := -lboost_program_options -lboost_system -pthread

SRC_DIR := src
BUILD_DIR := build
TEST_DIR := tests

# Source files
SRC := $(wildcard $(SRC_DIR)/*.cpp)
OBJ := $(patsubst $(SRC_DIR)/%.cpp,$(BUILD_DIR)/%.o,$(SRC))

# Main executable
MAIN := $(BUILD_DIR)/main

# Test files
TESTS := $(wildcard $(TEST_DIR)/*.cpp)
TEST_EXEC := $(patsubst $(TEST_DIR)/%.cpp,$(BUILD_DIR)/%,$(TESTS))

.PHONY: build tests clean

build: $(MAIN)

tests: $(TEST_EXEC)
	@for test in $^; do \
		echo "Running test: $$test"; \
		$$test; \
	done

$(MAIN): $(OBJ) main.cpp
	@mkdir -p $(BUILD_DIR)
	$(CXX) $(CXXFLAGS) $^ -o $@ $(LDFLAGS)

$(BUILD_DIR)/%.o: $(SRC_DIR)/%.cpp $(SRC_DIR)/%.hpp
	@mkdir -p $(BUILD_DIR)
	$(CXX) $(CXXFLAGS) -c $< -o $@

$(BUILD_DIR)/%: $(TEST_DIR)/%.cpp $(OBJ)
	@mkdir -p $(BUILD_DIR)
	$(CXX) $(CXXFLAGS) $^ -o $@ $(LDFLAGS)

clean:
	rm -rf $(BUILD_DIR)

