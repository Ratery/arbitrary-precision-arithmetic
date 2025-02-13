CXX = g++-14

CXXFLAGS =     \
	-c         \
	-std=c++23 \
	-Wall      \
	-Wextra    \
	-Werror

BUILD_PATH = build
SRC_PATH = src

all: pi

calculate-pi: $(BUILD_PATH)/calculate-pi.o $(BUILD_PATH)/LongNum.o | $(BUILD_PATH)
	$(CXX) $(BUILD_PATH)/calculate-pi.o $(BUILD_PATH)/LongNum.o -o $(BUILD_PATH)/calculate-pi

pi: calculate-pi
	$(BUILD_PATH)/calculate-pi 100

$(BUILD_PATH):
	mkdir -p $(BUILD_PATH)

$(BUILD_PATH)/LongNum.o: $(SRC_PATH)/LongNum.cpp $(SRC_PATH)/LongNum.hpp | $(BUILD_PATH)
	$(CXX) $(CXXFLAGS) $(SRC_PATH)/LongNum.cpp -o $(BUILD_PATH)/LongNum.o

$(BUILD_PATH)/calculate-pi.o: $(SRC_PATH)/calculate-pi.cpp | $(BUILD_PATH)
	$(CXX) $(CXXFLAGS) $(SRC_PATH)/calculate-pi.cpp -o $(BUILD_PATH)/calculate-pi.o

clean:
	rm -rf $(BUILD_PATH)
