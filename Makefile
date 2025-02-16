CXX = g++-14

CXXFLAGS += -std=c++23 -pedantic -Wall -Wextra -Werror
LDFLAGS ?=

BUILD_PATH ?= build

RELEASE ?= 0
ifeq ($(RELEASE), 0)
	CXXFLAGS += -g
else
	CXXFLAGS += -O3 -flto -DNDEBUG
endif

COMPILE = $(CXX) $(CXXFLAGS)
LINK = $(CXX) $(LDFLAGS)

all: $(BUILD_PATH)/longnum.o

PRECISION ?= 100
pi: $(BUILD_PATH)/calculate-pi
	$(BUILD_PATH)/calculate-pi $(PRECISION)

test: $(BUILD_PATH)/tests
	$(BUILD_PATH)/tests

$(BUILD_PATH):
	@mkdir -p $(BUILD_PATH)

$(BUILD_PATH)/calculate-pi: $(BUILD_PATH)/calculate-pi.o $(BUILD_PATH)/longnum.o | $(BUILD_PATH)
	$(LINK) $^ -o $@

$(BUILD_PATH)/longnum.o: src/longnum.cpp src/longnum.hpp | $(BUILD_PATH)
	$(COMPILE) $< -c -o $@

$(BUILD_PATH)/calculate-pi.o: src/calculate-pi.cpp src/longnum.hpp | $(BUILD_PATH)
	$(COMPILE) $< -c -o $@

$(BUILD_PATH)/tests: $(BUILD_PATH)/tests.o $(BUILD_PATH)/tester.o $(BUILD_PATH)/longnum.o | $(BUILD_PATH)
	$(LINK) $^ -o $@

$(BUILD_PATH)/tester.o: tests/tester.cpp tests/tester.hpp | $(BUILD_PATH)
	$(COMPILE) $< -c -o $@

$(BUILD_PATH)/tests.o: tests/tests.cpp tests/tester.hpp src/longnum.hpp | $(BUILD_PATH)
	$(COMPILE) $< -c -o $@

clean:
	rm -rf $(BUILD_PATH)

.PHONY: all pi test clean
