# Compiler settings
CC = clang++
DEBUG_FLAGS = -g -O0 -DDEBUG
RELEASE_FLAGS = -O3 -DNDEBUG
CFLAGS = -Wall -Wextra -fdiagnostics-color -pedantic-errors -std=c++20
ifeq ($(RELEASE), 1)
	CFLAGS += $(RELEASE_FLAGS)
else
	CFLAGS += $(DEBUG_FLAGS)
endif

OUT_DIR = build-out
PROJ_NAME = $(OUT_DIR)/poac


.PHONY: all clean test

all: $(OUT_DIR) $(OUT_DIR)/Cmd $(PROJ_NAME)

clean:
	rm -rf $(OUT_DIR)

$(OUT_DIR):
	mkdir -p $@

$(OUT_DIR)/Cmd:
	mkdir -p $@

$(PROJ_NAME): $(OUT_DIR)/Algos.o $(OUT_DIR)/Cmd/Build.o $(OUT_DIR)/Cmd/Test.o $(OUT_DIR)/Cmd/Run.o $(OUT_DIR)/BuildConfig.o $(OUT_DIR)/Logger.o $(OUT_DIR)/TermColor.o $(OUT_DIR)/main.o
	$(CC) $(CFLAGS) $^ -o $@

$(OUT_DIR)/Algos.o: src/Algos.cc src/Algos.hpp
	$(CC) $(CFLAGS) -c $< -o $@

$(OUT_DIR)/TermColor.o: src/TermColor.cc src/TermColor.hpp
	$(CC) $(CFLAGS) -c $< -o $@

$(OUT_DIR)/Logger.o: src/Logger.cc src/Logger.hpp
	$(CC) $(CFLAGS) -c $< -o $@

$(OUT_DIR)/Cmd/Build.o: src/Cmd/Build.cc src/Cmd/Build.hpp src/Rustify.hpp src/Algos.hpp src/Logger.hpp src/TermColor.hpp
	$(CC) $(CFLAGS) -c $< -o $@

$(OUT_DIR)/Cmd/Test.o: src/Cmd/Test.cc src/Cmd/Test.hpp src/Rustify.hpp src/Algos.hpp src/Logger.hpp src/TermColor.hpp
	$(CC) $(CFLAGS) -c $< -o $@

$(OUT_DIR)/Cmd/Run.o: src/Cmd/Run.cc src/Cmd/Run.hpp src/Rustify.hpp src/Algos.hpp src/Logger.hpp src/TermColor.hpp
	$(CC) $(CFLAGS) -c $< -o $@

$(OUT_DIR)/BuildConfig.o: src/BuildConfig.cc src/BuildConfig.hpp
	$(CC) $(CFLAGS) -c $< -o $@

$(OUT_DIR)/main.o: src/main.cc src/Cmd/Build.hpp
	$(CC) $(CFLAGS) -c $< -o $@


test: $(OUT_DIR)/tests $(OUT_DIR)/tests/test_BuildConfig $(OUT_DIR)/tests/test_Algos
	$(OUT_DIR)/tests/test_BuildConfig
	$(OUT_DIR)/tests/test_Algos

$(OUT_DIR)/tests:
	mkdir -p $@

$(OUT_DIR)/tests/test_BuildConfig: $(OUT_DIR)/tests/test_BuildConfig.o $(OUT_DIR)/Logger.o $(OUT_DIR)/TermColor.o
	$(CC) $(CFLAGS) $^ -o $@

$(OUT_DIR)/tests/test_BuildConfig.o: src/BuildConfig.cc src/BuildConfig.hpp src/Rustify.hpp src/Algos.hpp src/Logger.hpp src/TermColor.hpp
	$(CC) $(CFLAGS) -DPOAC_TEST -c $< -o $@

$(OUT_DIR)/tests/test_Algos: $(OUT_DIR)/tests/test_Algos.o $(OUT_DIR)/Logger.o $(OUT_DIR)/TermColor.o
	$(CC) $(CFLAGS) $^ -o $@

$(OUT_DIR)/tests/test_Algos.o: src/Algos.cc src/Algos.hpp src/Logger.hpp src/TermColor.hpp
	$(CC) $(CFLAGS) -DPOAC_TEST -c $< -o $@
