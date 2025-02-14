CXX = g++-14

CXXFLAGS =     \
	-c         \
	-std=c++23 \
	-Wall      \
	-Wextra    \
	-Werror

BUILD_PATH = build
SRC_PATH = src

pi: calculate-pi
	$(BUILD_PATH)/calculate-pi 100

test: tests
	$(BUILD_PATH)/tests

calculate-pi: $(BUILD_PATH)/calculate-pi.o $(BUILD_PATH)/LongNum.o | $(BUILD_PATH)
	$(CXX) $(BUILD_PATH)/calculate-pi.o $(BUILD_PATH)/LongNum.o -o $(BUILD_PATH)/calculate-pi

tests: $(BUILD_PATH)/tests.o $(BUILD_PATH)/LongNum.o $(BUILD_PATH)/tester.o | $(BUILD_PATH)
	$(CXX) $(BUILD_PATH)/tests.o $(BUILD_PATH)/LongNum.o $(BUILD_PATH)/tester.o -o $(BUILD_PATH)/tests

$(BUILD_PATH):
	mkdir -p $(BUILD_PATH)

$(BUILD_PATH)/LongNum.o: src/LongNum.cpp src/LongNum.hpp | $(BUILD_PATH)
	$(CXX) $(CXXFLAGS) src/LongNum.cpp -o $(BUILD_PATH)/LongNum.o

$(BUILD_PATH)/calculate-pi.o: $(SRC_PATH)/calculate-pi.cpp | $(BUILD_PATH)
	$(CXX) $(CXXFLAGS) src/calculate-pi.cpp -o src/calculate-pi.o

$(BUILD_PATH)/tester.o: tests/tester.cpp tests/tester.hpp | $(BUILD_PATH)
	$(CXX) $(CXXFLAGS) tests/tester.cpp -o $(BUILD_PATH)/tester.o

$(BUILD_PATH)/tests.o: tests/tests.cpp | $(BUILD_PATH)
	$(CXX) $(CXXFLAGS) tests/tests.cpp -o $(BUILD_PATH)/tests.o

clean:
	rm -rf $(BUILD_PATH)
