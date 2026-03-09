SHELL := /bin/bash

CC ?= cc
PKG_CONFIG ?= pkg-config

SRC_DIR := src
INC_DIR := include
BUILD_DIR := build
TARGET := $(BUILD_DIR)/knapsack

MODE ?= release
SAN ?= none

VALID_MODES := debug release
VALID_SANS := none asan

ifeq ($(filter $(MODE),$(VALID_MODES)),)
$(error MODE must be one of: $(VALID_MODES))
endif

ifeq ($(filter $(SAN),$(VALID_SANS)),)
$(error SAN must be one of: $(VALID_SANS))
endif

FLAVOR := $(MODE)$(if $(filter asan,$(SAN)),-asan,)
MODE_DIR := $(BUILD_DIR)/$(FLAVOR)
OBJ_DIR := $(MODE_DIR)/obj
BIN := $(MODE_DIR)/knapsack

SRCS := $(wildcard $(SRC_DIR)/*.c)
OBJS := $(patsubst $(SRC_DIR)/%.c,$(OBJ_DIR)/%.o,$(SRCS))
DEPS := $(OBJS:.o=.d)

BASE_CPPFLAGS := -I$(INC_DIR)
BASE_CFLAGS := -std=c11 -Wall -Wextra -D_POSIX_C_SOURCE=200809L

ifeq ($(MODE),debug)
MODE_CFLAGS := -g -O0
else
MODE_CFLAGS := -O2 -DNDEBUG
endif

ifeq ($(SAN),asan)
SAN_CFLAGS := -fsanitize=address,undefined -fno-omit-frame-pointer
SAN_LDFLAGS := -fsanitize=address,undefined
else
SAN_CFLAGS :=
SAN_LDFLAGS :=
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
CFLAGS += $(BASE_CFLAGS) $(MODE_CFLAGS) $(SAN_CFLAGS)
LDFLAGS += $(SAN_LDFLAGS)
LDLIBS += $(GMP_LIBS)

.PHONY: all debug release san clean help

all: $(TARGET)

debug:
	$(MAKE) MODE=debug SAN=none "$(BUILD_DIR)/debug/knapsack"
	ln -sfn "debug/knapsack" "$(TARGET)"

release:
	$(MAKE) MODE=release SAN=none "$(BUILD_DIR)/release/knapsack"
	ln -sfn "release/knapsack" "$(TARGET)"

san:
	$(MAKE) MODE=debug SAN=asan "$(BUILD_DIR)/debug-asan/knapsack"
	ln -sfn "debug-asan/knapsack" "$(TARGET)"

$(TARGET): $(BIN) | $(BUILD_DIR)
	ln -sfn "$(FLAVOR)/knapsack" "$@"

$(BIN): $(OBJS) | $(MODE_DIR)
	$(CC) $(OBJS) $(LDFLAGS) $(LDLIBS) -o "$@"

$(OBJ_DIR)/%.o: $(SRC_DIR)/%.c | $(OBJ_DIR)
	$(CC) $(CPPFLAGS) $(CFLAGS) -MMD -MP -c "$<" -o "$@"

$(BUILD_DIR) $(MODE_DIR) $(OBJ_DIR):
	mkdir -p "$@"

clean:
	rm -rf "$(BUILD_DIR)"

help:
	@printf "Targets:\n"
	@printf "  make [MODE=release|debug] [SAN=none|asan]  Build selected flavor\n"
	@printf "  make debug                                  Build debug (no sanitizer)\n"
	@printf "  make release                                Build release (no sanitizer)\n"
	@printf "  make san                                    Build debug with ASan/UBSan\n"
	@printf "  make clean                                  Remove build artifacts\n"
	@printf "  make help                                   Show this help\n"

-include $(DEPS)
