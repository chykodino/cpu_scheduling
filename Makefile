# ============================================================================
# CPU Scheduler Simulator - Makefile
# ============================================================================
# This Makefile supports building, testing, debugging, and cleaning the
# CPU scheduler simulation project.
#
# Available targets:
#   make build   - Compile the project with optimization
#   make debug   - Compile with debug symbols
#   make test    - Run unit and system tests
#   make clean   - Remove all build artifacts
#   make install - Prepare final executables
#   make run     - Build and run the simulator
#   make all     - Build everything (default target)
# ============================================================================

# Compiler and flags
CXX = g++
CXXFLAGS = -std=c++17 -Wall -Wextra -Wpedantic
INCLUDES = -Iinclude
LDFLAGS =

# Directories
SRC_DIR = src
INCLUDE_DIR = include
TEST_DIR = test
BUILD_DIR = build
BIN_DIR = bin
DOC_DIR = doc

# Source files
SOURCES = $(wildcard $(SRC_DIR)/*.cpp)
OBJECTS = $(SOURCES:$(SRC_DIR)/%.cpp=$(BUILD_DIR)/%.o)
MAIN_OBJ = $(BUILD_DIR)/main.o
LIB_OBJECTS = $(filter-out $(MAIN_OBJ), $(OBJECTS))

# Test files
TEST_SOURCES = $(wildcard $(TEST_DIR)/*.cpp)
TEST_OBJECTS = $(TEST_SOURCES:$(TEST_DIR)/%.cpp=$(BUILD_DIR)/test_%.o)

# Executables
EXECUTABLE = $(BIN_DIR)/scheduler_sim
TEST_EXECUTABLE = $(BIN_DIR)/test_runner

# Colors for output
COLOR_RESET = \033[0m
COLOR_GREEN = \033[32m
COLOR_YELLOW = \033[33m
COLOR_BLUE = \033[34m

# ============================================================================
# Default target
# ============================================================================
.PHONY: all
all: build

# ============================================================================
# Build target - Optimized compilation
# ============================================================================
.PHONY: build
build: CXXFLAGS += -O2
build: directories $(EXECUTABLE)
	@echo "$(COLOR_GREEN)✓ Build complete: $(EXECUTABLE)$(COLOR_RESET)"

# ============================================================================
# Debug target - Compilation with debug symbols
# ============================================================================
.PHONY: debug
debug: CXXFLAGS += -g -O0 -DDEBUG
debug: directories $(EXECUTABLE)
	@echo "$(COLOR_YELLOW)✓ Debug build complete: $(EXECUTABLE)$(COLOR_RESET)"

# ============================================================================
# Test target - Build and run tests
# ============================================================================
.PHONY: test
test: CXXFLAGS += -g -O0
test: directories $(TEST_EXECUTABLE)
	@echo "$(COLOR_BLUE)Running tests...$(COLOR_RESET)"
	@./$(TEST_EXECUTABLE)
	@echo "$(COLOR_GREEN)✓ All tests passed$(COLOR_RESET)"

# ============================================================================
# Clean target - Remove all build artifacts
# ============================================================================
.PHONY: clean
clean:
	@echo "$(COLOR_YELLOW)Cleaning build artifacts...$(COLOR_RESET)"
	@rm -rf $(BUILD_DIR) $(BIN_DIR)
	@echo "$(COLOR_GREEN)✓ Clean complete$(COLOR_RESET)"

# ============================================================================
# Install target - Prepare executables for deployment
# ============================================================================
.PHONY: install
install: build
	@echo "$(COLOR_BLUE)Installing executable...$(COLOR_RESET)"
	@mkdir -p /usr/local/bin 2>/dev/null || mkdir -p ~/bin
	@cp $(EXECUTABLE) ~/bin/scheduler_sim 2>/dev/null || \
		(echo "$(COLOR_YELLOW)Note: Installed to ~/bin/scheduler_sim$(COLOR_RESET)" && \
		 cp $(EXECUTABLE) ~/bin/)
	@echo "$(COLOR_GREEN)✓ Installation complete$(COLOR_RESET)"

# ============================================================================
# Run target - Build and execute the simulator
# ============================================================================
.PHONY: run
run: build
	@echo "$(COLOR_BLUE)Running CPU Scheduler Simulator...$(COLOR_RESET)"
	@./$(EXECUTABLE)

# ============================================================================
# Create necessary directories
# ============================================================================
.PHONY: directories
directories:
	@mkdir -p $(BUILD_DIR) $(BIN_DIR)

# ============================================================================
# Link executable
# ============================================================================
$(EXECUTABLE): $(OBJECTS)
	@echo "$(COLOR_BLUE)Linking executable: $@$(COLOR_RESET)"
	@$(CXX) $(OBJECTS) -o $@ $(LDFLAGS)

# ============================================================================
# Link test executable
# ============================================================================
$(TEST_EXECUTABLE): $(LIB_OBJECTS) $(TEST_OBJECTS)
	@echo "$(COLOR_BLUE)Linking test executable: $@$(COLOR_RESET)"
	@$(CXX) $(LIB_OBJECTS) $(TEST_OBJECTS) -o $@ $(LDFLAGS)

# ============================================================================
# Compile source files
# ============================================================================
$(BUILD_DIR)/%.o: $(SRC_DIR)/%.cpp
	@echo "$(COLOR_BLUE)Compiling: $<$(COLOR_RESET)"
	@$(CXX) $(CXXFLAGS) $(INCLUDES) -c $< -o $@

# ============================================================================
# Compile test files
# ============================================================================
$(BUILD_DIR)/test_%.o: $(TEST_DIR)/%.cpp
	@echo "$(COLOR_BLUE)Compiling test: $<$(COLOR_RESET)"
	@$(CXX) $(CXXFLAGS) $(INCLUDES) -c $< -o $@

# ============================================================================
# Help target - Display available commands
# ============================================================================
.PHONY: help
help:
	@echo "$(COLOR_GREEN)CPU Scheduler Simulator - Makefile Targets$(COLOR_RESET)"
	@echo ""
	@echo "  make build    - Build optimized executable"
	@echo "  make debug    - Build with debug symbols"
	@echo "  make test     - Build and run tests"
	@echo "  make clean    - Remove build artifacts"
	@echo "  make install  - Install executable to ~/bin"
	@echo "  make run      - Build and run simulator"
	@echo "  make help     - Display this help message"
	@echo ""

# ============================================================================
# Dependencies (auto-generated would go here in production)
# ============================================================================
$(BUILD_DIR)/Process.o: $(INCLUDE_DIR)/Process.h
$(BUILD_DIR)/Scheduler.o: $(INCLUDE_DIR)/Scheduler.h $(INCLUDE_DIR)/Process.h
$(BUILD_DIR)/RoundRobinScheduler.o: $(INCLUDE_DIR)/RoundRobinScheduler.h $(INCLUDE_DIR)/Scheduler.h
$(BUILD_DIR)/PriorityScheduler.o: $(INCLUDE_DIR)/PriorityScheduler.h $(INCLUDE_DIR)/Scheduler.h
$(BUILD_DIR)/MultilevelQueueScheduler.o: $(INCLUDE_DIR)/MultilevelQueueScheduler.h $(INCLUDE_DIR)/Scheduler.h
$(BUILD_DIR)/MultilevelFeedbackQueueScheduler.o: $(INCLUDE_DIR)/MultilevelFeedbackQueueScheduler.h $(INCLUDE_DIR)/Scheduler.h
$(BUILD_DIR)/main.o: $(INCLUDE_DIR)/*.h
