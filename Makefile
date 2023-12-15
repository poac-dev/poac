# Compiler settings
CC = clang++
# CFLAGS = -Wall -Wextra -Werror -fdiagnostics-color -pedantic-errors -std=c++20
CFLAGS = -Wextra -fdiagnostics-color -pedantic-errors -std=c++20
LDFLAGS = -L.

DEBUG_FLAGS = -g -O0 -DDEBUG
RELEASE_FLAGS = -O3 -DNDEBUG

# Archiver settings
AR = ar
ARFLAGS = rcs

# Directories
OUT_DIR = build-out

# Project settings
PROJ_NAME = $(OUT_DIR)/poac


all: $(PROJ_NAME)

clean:
	rm -rf $(OUT_DIR)

$(PROJ_NAME): $(OUT_DIR)/main.o $(OUT_DIR)/Build.o | $(OUT_DIR)
	$(CC) $(CFLAGS) $^ -o $@

$(OUT_DIR)/Build.o: src/Build.cc src/Build.hpp src/Util/Rustify.hpp src/Util/Algos.hpp
	$(CC) $(CFLAGS) -c $< -o $@

$(OUT_DIR)/main.o: src/main.cc src/Build.hpp
	$(CC) $(CFLAGS) -c $< -o $@

$(OUT_DIR):
	mkdir -p $(OUT_DIR)

.PHONY: all clean
