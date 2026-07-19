CC ?= cc
AR ?= ar
CFLAGS ?= -O3 -std=c11 -Wall -Wextra -Wpedantic
CPPFLAGS ?= -Isrc/include

BUILD_DIR := build
LIB := $(BUILD_DIR)/libareion.a
OBJ := $(BUILD_DIR)/areion.o
TEST := $(BUILD_DIR)/test-areion
BENCH := $(BUILD_DIR)/benchmark

.PHONY: all test benchmark clean

all: $(LIB)

$(BUILD_DIR):
	mkdir -p $@

$(OBJ): src/areion.c src/include/areion.h | $(BUILD_DIR)
	$(CC) $(CPPFLAGS) $(CFLAGS) -c $< -o $@

$(LIB): $(OBJ)
	$(AR) rcs $@ $^

$(TEST): src/test/main.c $(LIB)
	$(CC) $(CPPFLAGS) $(CFLAGS) $< $(LIB) -o $@

test: $(TEST)
	$(TEST)

$(BENCH): src/test/benchmark.c $(LIB)
	$(CC) $(CPPFLAGS) $(CFLAGS) $< $(LIB) -o $@

benchmark: $(BENCH)
	$(BENCH)

clean:
	rm -rf $(BUILD_DIR)
