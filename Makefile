CXX      = g++
CXXFLAGS = -std=c++17 -Wall -Wextra -Iinclude

# Folder layout:
#   include/  all .h files
#   src/      all .cpp files except main.cpp
#   build/    intermediate .o files (created automatically)
#   the finished program is written to the project root
SRC_DIR   = src
BUILD_DIR = build

# On Windows (MinGW/MSYS2) the program is an .exe
ifeq ($(OS),Windows_NT)
  EXE = .exe
endif
TARGET = ecommerce$(EXE)

# Every .cpp in src/ is picked up automatically, plus main.cpp
SRC_FILES = $(wildcard $(SRC_DIR)/*.cpp)
OBJS = $(BUILD_DIR)/main.o $(SRC_FILES:$(SRC_DIR)/%.cpp=$(BUILD_DIR)/%.o)
DEPS = $(OBJS:.o=.d)

all: $(TARGET)

# Link step: build/*.o  ->  ecommerce
$(TARGET): $(OBJS)
	$(CXX) $(CXXFLAGS) -o $@ $(OBJS)

# Compile steps. -MMD also writes build/X.d, a list of the headers X.cpp uses,
# so editing a .h file correctly triggers a rebuild.
$(BUILD_DIR)/main.o: main.cpp | $(BUILD_DIR)
	$(CXX) $(CXXFLAGS) -MMD -MP -c $< -o $@

$(BUILD_DIR)/%.o: $(SRC_DIR)/%.cpp | $(BUILD_DIR)
	$(CXX) $(CXXFLAGS) -MMD -MP -c $< -o $@

$(BUILD_DIR):
	mkdir -p $@

# Memory-safety check (AddressSanitizer + UBSan) -> ecommerce_asan
asan:
	$(CXX) $(CXXFLAGS) -g -fsanitize=address,undefined -o ecommerce_asan$(EXE) main.cpp $(SRC_FILES)

clean:
	rm -rf $(BUILD_DIR) $(TARGET) ecommerce_asan$(EXE)

-include $(DEPS)

.PHONY: all asan clean
