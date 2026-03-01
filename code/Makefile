SHELL := /bin/bash

CC ?= cc
PKG_CONFIG ?= pkg-config

SRC_DIR := src
BUILD_DIR := build
OBJ_DIR := $(BUILD_DIR)/obj
TARGET := $(BUILD_DIR)/knapsack
MODE_FILE := $(BUILD_DIR)/.build_mode

SRCS := $(wildcard $(SRC_DIR)/*.c)
OBJS := $(patsubst $(SRC_DIR)/%.c,$(OBJ_DIR)/%.o,$(SRCS))
DEPS := $(OBJS:.o=.d)

BASE_CFLAGS := -std=c11 -Wall -Wextra
BASE_CPPFLAGS := -Iinclude

MODE ?= release
ifeq ($(MODE),debug)
MODE_CFLAGS := -g -O0
else ifeq ($(MODE),release)
MODE_CFLAGS := -O2 -DNDEBUG
else
$(error MODE must be 'release' or 'debug')
endif

GMP_CFLAGS := $(shell $(PKG_CONFIG) --cflags gmp 2>/dev/null)
GMP_LIBS := $(shell $(PKG_CONFIG) --libs gmp 2>/dev/null)

ifeq ($(strip $(GMP_LIBS)),)
UNAME_S := $(shell uname -s)
ifeq ($(UNAME_S),Darwin)
ifneq ($(wildcard /opt/homebrew/opt/gmp/include/gmp.h),)
GMP_CFLAGS := -I/opt/homebrew/opt/gmp/include
GMP_LIBS := -L/opt/homebrew/opt/gmp/lib -lgmp
else ifneq ($(wildcard /usr/local/opt/gmp/include/gmp.h),)
GMP_CFLAGS := -I/usr/local/opt/gmp/include
GMP_LIBS := -L/usr/local/opt/gmp/lib -lgmp
else
GMP_LIBS := -lgmp
endif
else
GMP_LIBS := -lgmp
endif
endif

CPPFLAGS += $(BASE_CPPFLAGS) $(GMP_CFLAGS)
CFLAGS += $(BASE_CFLAGS) $(MODE_CFLAGS)
LDLIBS += $(GMP_LIBS)

.PHONY: all debug release clean run-demo run-bench help FORCE

all: $(MODE_FILE) $(TARGET)

debug: MODE=debug
debug: all

release: MODE=release
release: all

$(TARGET): $(MODE_FILE) $(OBJS) | $(BUILD_DIR)
	$(CC) $(OBJS) $(LDFLAGS) $(LDLIBS) -o $@

$(OBJ_DIR)/%.o: $(SRC_DIR)/%.c $(MODE_FILE) | $(OBJ_DIR)
	$(CC) $(CPPFLAGS) $(CFLAGS) -MMD -MP -c $< -o $@

$(MODE_FILE): FORCE | $(BUILD_DIR)
	@if [ -f "$(MODE_FILE)" ] && [ "$$(cat "$(MODE_FILE)")" != "$(MODE)" ]; then \
		rm -rf "$(OBJ_DIR)" "$(TARGET)"; \
	fi
	@mkdir -p "$(OBJ_DIR)"
	@printf "%s" "$(MODE)" > "$(MODE_FILE)"

$(BUILD_DIR):
	mkdir -p "$(BUILD_DIR)"

$(OBJ_DIR):
	mkdir -p "$(OBJ_DIR)"

run-demo: all
	./$(TARGET) demo

run-bench: all
	./$(TARGET) bench

clean:
	rm -rf "$(BUILD_DIR)"

help:
	@printf "Targets:\n"
	@printf "  make [MODE=release|debug]  Build $(TARGET)\n"
	@printf "  make debug                 Build debug binary\n"
	@printf "  make release               Build optimized binary\n"
	@printf "  make run-demo              Build and run demo mode\n"
	@printf "  make run-bench             Build and run bench mode\n"
	@printf "  make clean                 Remove build artifacts\n"

-include $(DEPS)
