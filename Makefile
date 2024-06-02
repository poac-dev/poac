CXX ?= clang++
POAC_TIDY ?= clang-tidy
PREFIX ?= /usr/local
INSTALL ?= install
COMMIT_HASH ?= $(shell git rev-parse HEAD)
COMMIT_SHORT_HASH ?= $(shell git rev-parse --short HEAD)
COMMIT_DATE ?= $(shell git show -s --date=format-local:'%Y-%m-%d' --format=%cd)

DEBUG_FLAGS := -g -O0 -DDEBUG
RELEASE_FLAGS := -O3 -DNDEBUG -flto
CXXFLAGS := -std=c++20 -fdiagnostics-color -pedantic-errors -Wall -Wextra -Wpedantic
ifeq ($(RELEASE), 1)
	CXXFLAGS += $(RELEASE_FLAGS)
else
	CXXFLAGS += $(DEBUG_FLAGS)
endif

O := build-out
PROJECT := $(O)/poac
VERSION := $(shell grep -m1 version poac.toml | cut -f 2 -d'"')
MKDIR_P := @mkdir -p

DEFINES := -DPOAC_PKG_VERSION='"$(VERSION)"' \
  -DPOAC_COMMIT_HASH='"$(COMMIT_HASH)"' \
  -DPOAC_COMMIT_SHORT_HASH='"$(COMMIT_SHORT_HASH)"' \
  -DPOAC_COMMIT_DATE='"$(COMMIT_DATE)"'
INCLUDES := -isystem $(O)/DEPS/toml11 \
  $(shell pkg-config --cflags 'libgit2 >= 1.1.0, libgit2 < 2.0.0') \
  $(shell pkg-config --cflags 'libcurl >= 7.79.1, libcurl < 9.0.0') \
  $(shell pkg-config --cflags 'nlohmann_json >= 3.10.5, nlohmann_json < 4.0.0') \
  $(shell pkg-config --cflags 'tbb >= 2021.5.0, tbb < 2022.0.0') \
  $(shell pkg-config --cflags 'fmt >= 8.1.1, fmt < 11.0.0')
LIBS := $(shell pkg-config --libs 'libgit2 >= 1.1.0, libgit2 < 2.0.0') \
  $(shell pkg-config --libs 'libcurl >= 7.79.1, libcurl < 9.0.0') \
  $(shell pkg-config --libs 'tbb >= 2021.5.0, tbb < 2022.0.0') \
  $(shell pkg-config --libs 'fmt >= 8.1.1, fmt < 11.0.0')

SRCS := $(shell find src -name '*.cc')
OBJS := $(patsubst src/%,$(O)/%,$(SRCS:.cc=.o))
DEPS := $(OBJS:.o=.d)

UNITTEST_SRCS := src/BuildConfig.cc src/Algos.cc src/Semver.cc src/VersionReq.cc src/Manifest.cc
UNITTEST_OBJS := $(patsubst src/%,$(O)/tests/test_%,$(UNITTEST_SRCS:.cc=.o))
UNITTEST_BINS := $(UNITTEST_OBJS:.o=)
UNITTEST_DEPS := $(UNITTEST_OBJS:.o=.d)

TIDY_TARGETS := $(patsubst src/%,tidy_%,$(SRCS))

GIT_DEPS := $(O)/DEPS/toml11


.PHONY: all clean install test versions tidy $(TIDY_TARGETS)


all: $(PROJECT)

$(PROJECT): $(OBJS)
	$(CXX) $(CXXFLAGS) $^ $(LIBS) $(LDFLAGS) -o $@

$(O)/%.o: src/%.cc $(GIT_DEPS)
	$(MKDIR_P) $(@D)
	$(CXX) $(CXXFLAGS) -MMD $(DEFINES) $(INCLUDES) -c $< -o $@

-include $(DEPS)


test: $(UNITTEST_BINS)
	@$(O)/tests/test_BuildConfig
	@$(O)/tests/test_Algos
	@$(O)/tests/test_Semver
	@$(O)/tests/test_VersionReq
	@$(O)/tests/test_Manifest

$(O)/tests/test_%.o: src/%.cc $(GIT_DEPS)
	$(MKDIR_P) $(@D)
	$(CXX) $(CXXFLAGS) -MMD -DPOAC_TEST $(DEFINES) $(INCLUDES) -c $< -o $@

-include $(UNITTEST_DEPS)

$(O)/tests/test_BuildConfig: $(O)/tests/test_BuildConfig.o $(O)/Algos.o \
  $(O)/TermColor.o $(O)/Manifest.o $(O)/Parallelism.o $(O)/Semver.o \
  $(O)/VersionReq.o $(O)/Git2/Repository.o $(O)/Git2/Object.o $(O)/Git2/Oid.o \
  $(O)/Git2/Global.o $(O)/Git2/Config.o $(O)/Git2/Exception.o $(O)/Git2/Time.o \
  $(O)/Git2/Commit.o
	$(CXX) $(CXXFLAGS) $^ $(LIBS) $(LDFLAGS) -o $@

$(O)/tests/test_Algos: $(O)/tests/test_Algos.o $(O)/TermColor.o
	$(CXX) $(CXXFLAGS) $^ $(LIBS) $(LDFLAGS) -o $@

$(O)/tests/test_Semver: $(O)/tests/test_Semver.o $(O)/TermColor.o
	$(CXX) $(CXXFLAGS) $^ $(LIBS) $(LDFLAGS) -o $@

$(O)/tests/test_VersionReq: $(O)/tests/test_VersionReq.o $(O)/TermColor.o \
  $(O)/Semver.o
	$(CXX) $(CXXFLAGS) $^ $(LIBS) $(LDFLAGS) -o $@

$(O)/tests/test_Manifest: $(O)/tests/test_Manifest.o $(O)/TermColor.o \
  $(O)/Semver.o $(O)/VersionReq.o $(O)/Algos.o $(O)/Git2/Repository.o \
  $(O)/Git2/Global.o $(O)/Git2/Oid.o $(O)/Git2/Config.o $(O)/Git2/Exception.o \
  $(O)/Git2/Object.o
	$(CXX) $(CXXFLAGS) $^ $(LIBS) $(LDFLAGS) -o $@


tidy: $(TIDY_TARGETS)

$(TIDY_TARGETS): tidy_%: src/% $(GIT_DEPS)
	$(POAC_TIDY) $(POAC_TIDY_FLAGS) $< -- $(CXXFLAGS) $(DEFINES) -DPOAC_TEST $(INCLUDES)

install: all
	$(INSTALL) -m 0755 -d $(DESTDIR)$(PREFIX)/bin
	$(INSTALL) -m 0755 $(PROJECT) $(DESTDIR)$(PREFIX)/bin

clean:
	-rm -rf $(O)

versions:
	$(MAKE) -v
	$(CXX) --version

# Git dependencies
$(O)/DEPS/toml11:
	$(MKDIR_P) $(@D)
	git clone https://github.com/ToruNiina/toml11.git $@
	git -C $@ reset --hard v3.8.0
