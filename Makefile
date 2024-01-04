CXX ?= clang++
PREFIX ?= /usr/local
INSTALL ?= install

DEBUG_FLAGS := -g -O0 -DDEBUG
RELEASE_FLAGS := -O3 -DNDEBUG -flto
CXXFLAGS := -Wall -Wextra -fdiagnostics-color -pedantic-errors -std=c++20
ifeq ($(RELEASE), 1)
	CXXFLAGS += $(RELEASE_FLAGS)
else
	CXXFLAGS += $(DEBUG_FLAGS)
endif

OUT_DIR := build-out
PROJ_NAME := $(OUT_DIR)/poac
VERSION := $(shell grep -m1 version poac.toml | cut -f 2 -d'"')

DEFINES := -DPOAC_VERSION='"$(VERSION)"'
INCLUDES := -I$(OUT_DIR)/DEPS/toml11 $(shell pkg-config --cflags 'libgit2 >= 1.1.0, libgit2 < 2.0.0')
LIBS := $(shell pkg-config --libs 'libgit2 >= 1.1.0, libgit2 < 2.0.0')

SRCS := $(shell find src -name '*.cc')
OBJS := $(patsubst src/%,$(OUT_DIR)/%,$(SRCS:.cc=.o))
DEPS := $(OBJS:.o=.d)

UNITTEST_SRCS := src/BuildConfig.cc src/Algos.cc src/Semver.cc src/VersionReq.cc
UNITTEST_OBJS := $(patsubst src/%,$(OUT_DIR)/tests/test_%,$(UNITTEST_SRCS:.cc=.o))
UNITTEST_BINS := $(UNITTEST_OBJS:.o=)
UNITTEST_DEPS := $(UNITTEST_OBJS:.o=.d)

OUTSIDE_DEPS := $(OUT_DIR)/DEPS/toml11


.PHONY: all clean install test

all: $(DEPS) $(PROJ_NAME)

clean:
	rm -rf $(OUT_DIR)

install: all
	$(INSTALL) -m 0755 -d $(DESTDIR)$(PREFIX)/bin
	$(INSTALL) -m 0755 $(PROJ_NAME) $(DESTDIR)$(PREFIX)/bin


$(OUT_DIR)/%.d: src/%.cc | $(OUTSIDE_DEPS) $(OUT_DIR) $(OUT_DIR)/Cmd
	$(CXX) $(CXXFLAGS) $(DEFINES) $(INCLUDES) -MM -MT $(@:.d=.o) $< -MF $@

-include $(DEPS)


$(OUT_DIR)/DEPS/toml11: $(OUT_DIR)/DEPS
	git clone https://github.com/ToruNiina/toml11.git $@
	git -C $@ reset --hard 01a0e93e5f9bef09a76d22f3d51a5257e3d923fe

$(OUT_DIR)/DEPS:
	mkdir -p $@


$(PROJ_NAME): $(OBJS)
	$(CXX) $(CXXFLAGS) $^ $(LIBS) -o $@

$(OUT_DIR)/%.o: src/%.cc
	$(CXX) $(CXXFLAGS) $(DEFINES) $(INCLUDES) -c $< -o $@


test: $(UNITTEST_DEPS) $(UNITTEST_BINS)
	@$(OUT_DIR)/tests/test_BuildConfig
	@$(OUT_DIR)/tests/test_Algos
	@$(OUT_DIR)/tests/test_Semver
	@$(OUT_DIR)/tests/test_VersionReq

$(OUT_DIR)/tests/test_%.d: src/%.cc | $(OUTSIDE_DEPS) $(OUT_DIR) $(OUT_DIR)/tests
	$(CXX) $(CXXFLAGS) -DPOAC_TEST $(DEFINES) $(INCLUDES) -MM -MT $(@:.d=.o) $< -MF $@

-include $(UNITTEST_DEPS)


$(OUT_DIR)/tests/test_BuildConfig: $(OUT_DIR)/tests/test_BuildConfig.o \
  $(OUT_DIR)/Logger.o $(OUT_DIR)/TermColor.o $(OUT_DIR)/Manifest.o \
  $(OUT_DIR)/Semver.o $(OUT_DIR)/Algos.o $(OUT_DIR)/VersionReq.o
	$(CXX) $(CXXFLAGS) $^ $(LIBS) -o $@

$(OUT_DIR)/tests/test_Algos: $(OUT_DIR)/tests/test_Algos.o $(OUT_DIR)/Logger.o \
  $(OUT_DIR)/TermColor.o
	$(CXX) $(CXXFLAGS) $^ $(LIBS) -o $@

$(OUT_DIR)/tests/test_Semver: $(OUT_DIR)/tests/test_Semver.o $(OUT_DIR)/Logger.o \
  $(OUT_DIR)/TermColor.o
	$(CXX) $(CXXFLAGS) $^ $(LIBS) -o $@

$(OUT_DIR)/tests/test_VersionReq: $(OUT_DIR)/tests/test_VersionReq.o \
  $(OUT_DIR)/Logger.o $(OUT_DIR)/TermColor.o $(OUT_DIR)/Semver.o
	$(CXX) $(CXXFLAGS) $^ $(LIBS) -o $@


$(OUT_DIR)/tests/test_%.o: src/%.cc
	$(CXX) $(CXXFLAGS) -DPOAC_TEST $(DEFINES) $(INCLUDES) -c $< -o $@


$(OUT_DIR):
	mkdir -p $@

$(OUT_DIR)/Cmd:
	mkdir -p $@

$(OUT_DIR)/tests:
	mkdir -p $@
