# Compiler settings
CXX ?= clang++
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

$(OUT_DIR)/DEPS/toml11: $(OUT_DIR)/DEPS
	git clone https://github.com/ToruNiina/toml11.git $@
	cd $@ && git reset --hard 846abd9a49082fe51440aa07005c360f13a67bbf

$(OUT_DIR)/DEPS:
	mkdir -p $@

$(OUT_DIR):
	mkdir -p $@

$(OUT_DIR)/Cmd:
	mkdir -p $@

$(PROJ_NAME): $(OUT_DIR)/Cmd/Help.o $(OUT_DIR)/Algos.o $(OUT_DIR)/Cmd/Build.o \
  $(OUT_DIR)/Cmd/Test.o $(OUT_DIR)/Cmd/Run.o $(OUT_DIR)/Cmd/New.o \
  $(OUT_DIR)/Cmd/Clean.o $(OUT_DIR)/Cmd/Init.o $(OUT_DIR)/Cmd/Version.o \
  $(OUT_DIR)/BuildConfig.o $(OUT_DIR)/Manifest.o $(OUT_DIR)/Logger.o \
  $(OUT_DIR)/TermColor.o $(OUT_DIR)/main.o
	$(CXX) $(CFLAGS) $^ -o $@

$(OUT_DIR)/Algos.o: src/Algos.cc src/Algos.hpp
	$(CXX) $(CFLAGS) -c $< -o $@

$(OUT_DIR)/TermColor.o: src/TermColor.cc src/TermColor.hpp
	$(CXX) $(CFLAGS) -c $< -o $@

$(OUT_DIR)/Logger.o: src/Logger.cc src/Logger.hpp
	$(CXX) $(CFLAGS) -c $< -o $@

$(OUT_DIR)/Cmd/Help.o: src/Cmd/Help.cc src/Cmd/Help.hpp src/Rustify.hpp \
  src/Algos.hpp src/Logger.hpp src/TermColor.hpp
	$(CXX) $(CFLAGS) -c $< -o $@

$(OUT_DIR)/Cmd/Build.o: src/Cmd/Build.cc src/Cmd/Build.hpp src/Rustify.hpp \
  src/Algos.hpp src/Logger.hpp src/TermColor.hpp
	$(CXX) $(CFLAGS) -c $< -o $@

$(OUT_DIR)/Cmd/Test.o: src/Cmd/Test.cc src/Cmd/Test.hpp src/Rustify.hpp \
  src/Algos.hpp src/Logger.hpp src/TermColor.hpp
	$(CXX) $(CFLAGS) -c $< -o $@

$(OUT_DIR)/Cmd/Run.o: src/Cmd/Run.cc src/Cmd/Run.hpp src/Rustify.hpp \
  src/Algos.hpp src/Logger.hpp src/TermColor.hpp
	$(CXX) $(CFLAGS) -c $< -o $@

$(OUT_DIR)/Cmd/New.o: src/Cmd/New.cc src/Cmd/New.hpp src/Rustify.hpp \
  src/Algos.hpp src/Logger.hpp src/TermColor.hpp
	$(CXX) $(CFLAGS) -c $< -o $@

$(OUT_DIR)/Cmd/Clean.o: src/Cmd/Clean.cc src/Cmd/Clean.hpp src/Rustify.hpp \
  src/Algos.hpp src/Logger.hpp src/TermColor.hpp
	$(CXX) $(CFLAGS) -c $< -o $@

$(OUT_DIR)/Cmd/Init.o: src/Cmd/Init.cc src/Cmd/Init.hpp src/Rustify.hpp \
  src/Algos.hpp src/Logger.hpp src/TermColor.hpp
	$(CXX) $(CFLAGS) -c $< -o $@

$(OUT_DIR)/Cmd/Version.o: src/Cmd/Version.cc src/Cmd/Version.hpp \
  src/Rustify.hpp src/Algos.hpp src/Logger.hpp src/TermColor.hpp
	$(CXX) $(CFLAGS) -c $< -o $@

$(OUT_DIR)/BuildConfig.o: src/BuildConfig.cc src/BuildConfig.hpp
	$(CXX) $(CFLAGS) -c $< -o $@

$(OUT_DIR)/Manifest.o: src/Manifest.cc src/Manifest.hpp $(OUT_DIR)/DEPS/toml11
	$(CXX) $(CFLAGS) -c $< -o $@ -I$(OUT_DIR)/DEPS/toml11

$(OUT_DIR)/main.o: src/main.cc src/Cmd/Build.hpp src/Cmd/Test.hpp \
  src/Cmd/Run.hpp src/Cmd/New.hpp src/Cmd/Clean.hpp src/Cmd/Init.hpp \
  src/Cmd/Version.hpp src/Rustify.hpp src/Algos.hpp src/Logger.hpp \
  src/TermColor.hpp src/Cmd/Global.hpp
	$(CXX) $(CFLAGS) -c $< -o $@


test: $(OUT_DIR)/tests $(OUT_DIR)/tests/test_BuildConfig \
  $(OUT_DIR)/tests/test_Algos
	$(OUT_DIR)/tests/test_BuildConfig
	$(OUT_DIR)/tests/test_Algos

$(OUT_DIR)/tests:
	mkdir -p $@

$(OUT_DIR)/tests/test_BuildConfig: $(OUT_DIR)/tests/test_BuildConfig.o \
  $(OUT_DIR)/Logger.o $(OUT_DIR)/TermColor.o $(OUT_DIR)/Manifest.o
	$(CXX) $(CFLAGS) $^ -o $@

$(OUT_DIR)/tests/test_BuildConfig.o: src/BuildConfig.cc src/BuildConfig.hpp \
  src/Rustify.hpp src/Algos.hpp src/Logger.hpp src/TermColor.hpp
	$(CXX) $(CFLAGS) -DPOAC_TEST -c $< -o $@

$(OUT_DIR)/tests/test_Algos: $(OUT_DIR)/tests/test_Algos.o $(OUT_DIR)/Logger.o \
  $(OUT_DIR)/TermColor.o
	$(CXX) $(CFLAGS) $^ -o $@

$(OUT_DIR)/tests/test_Algos.o: src/Algos.cc src/Algos.hpp src/Logger.hpp \
  src/TermColor.hpp
	$(CXX) $(CFLAGS) -DPOAC_TEST -c $< -o $@
