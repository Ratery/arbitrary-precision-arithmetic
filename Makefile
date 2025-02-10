CXX = g++-14

CXXFLAGS =     \
	-c         \
	-std=c++23 \
	-Wall      \
	-Wextra    \
	-Werror

LDFLAGS =

BUILD_PATH = build
SRC_PATH = src

all: $(BUILD_PATH)/LongNum.o

$(BUILD_PATH):
	mkdir -p $(BUILD_PATH)

$(BUILD_PATH)/LongNum.o: $(SRC_PATH)/LongNum.cpp $(SRC_PATH)/LongNum.hpp | $(BUILD_PATH)
	$(CXX) $(CXXFLAGS) $(SRC_PATH)/LongNum.cpp -o $(BUILD_PATH)/LongNum.o

clean:
	rm -rf $(BUILD_PATH)
