CXX ?= clang++
POAC_TIDY ?= clang-tidy
PREFIX ?= /usr/local
INSTALL ?= install

DEBUG_FLAGS := -g -O0 -DDEBUG
RELEASE_FLAGS := -O3 -DNDEBUG -flto
CXXFLAGS := -std=c++2b -fdiagnostics-color -pedantic-errors -Wall -Wextra -Wpedantic
ifeq ($(RELEASE), 1)
	CXXFLAGS += $(RELEASE_FLAGS)
else
	CXXFLAGS += $(DEBUG_FLAGS)
endif

O := build-out
PROJECT := $(O)/poac
VERSION := $(shell grep -m1 version poac.toml | cut -f 2 -d'"')
MKDIR_P := @mkdir -p

DEFINES := -DPOAC_VERSION='"$(VERSION)"'
INCLUDES := -isystem $(O)/DEPS/toml11 \
  $(shell pkg-config --cflags 'libgit2 >= 1.1.0, libgit2 < 2.0.0') \
  $(shell pkg-config --cflags 'libcurl >= 7.79.1, libcurl < 9.0.0') \
  $(shell pkg-config --cflags 'nlohmann_json >= 3.10.5, nlohmann_json < 4.0.0')
LIBS := $(shell pkg-config --libs 'libgit2 >= 1.1.0, libgit2 < 2.0.0') \
  $(shell pkg-config --libs 'libcurl >= 7.79.1, libcurl < 9.0.0')

SRCS := $(shell find src -name '*.cc')
OBJS := $(patsubst src/%,$(O)/%,$(SRCS:.cc=.o))
DEPS := $(OBJS:.o=.d)

UNITTEST_SRCS := src/BuildConfig.cc src/Algos.cc src/Semver.cc src/VersionReq.cc
UNITTEST_OBJS := $(patsubst src/%,$(O)/tests/test_%,$(UNITTEST_SRCS:.cc=.o))
UNITTEST_BINS := $(UNITTEST_OBJS:.o=)
UNITTEST_DEPS := $(UNITTEST_OBJS:.o=.d)

GIT_DEPS := $(O)/DEPS/toml11


.PHONY: all clean install test tidy versions


all: $(GIT_DEPS) $(PROJECT)

$(PROJECT): $(OBJS)
	$(CXX) $(CXXFLAGS) $^ $(LIBS) -o $@

$(O)/%.o: src/%.cc
	$(MKDIR_P) $(@D)
	$(CXX) $(CXXFLAGS) -MMD $(DEFINES) $(INCLUDES) -c $< -o $@

-include $(DEPS)


test: $(GIT_DEPS) $(UNITTEST_BINS)
	@$(O)/tests/test_BuildConfig
	@$(O)/tests/test_Algos
	@$(O)/tests/test_Semver
	@$(O)/tests/test_VersionReq

$(O)/tests/test_%.o: src/%.cc
	$(MKDIR_P) $(@D)
	$(CXX) $(CXXFLAGS) -MMD -DPOAC_TEST $(DEFINES) $(INCLUDES) -c $< -o $@

-include $(UNITTEST_DEPS)

$(O)/tests/test_BuildConfig: $(O)/tests/test_BuildConfig.o \
  $(O)/Logger.o $(O)/TermColor.o $(O)/Manifest.o \
  $(O)/Semver.o $(O)/Algos.o $(O)/VersionReq.o $(O)/Git/Repository.o \
  $(O)/Git/Object.o $(O)/Git/Oid.o $(O)/Git/Global.o $(O)/Git/Config.o \
  $(O)/Git/Exception.o
	$(CXX) $(CXXFLAGS) $^ $(LIBS) -o $@

$(O)/tests/test_Algos: $(O)/tests/test_Algos.o $(O)/Logger.o \
  $(O)/TermColor.o
	$(CXX) $(CXXFLAGS) $^ $(LIBS) -o $@

$(O)/tests/test_Semver: $(O)/tests/test_Semver.o $(O)/Logger.o \
  $(O)/TermColor.o
	$(CXX) $(CXXFLAGS) $^ $(LIBS) -o $@

$(O)/tests/test_VersionReq: $(O)/tests/test_VersionReq.o \
  $(O)/Logger.o $(O)/TermColor.o $(O)/Semver.o
	$(CXX) $(CXXFLAGS) $^ $(LIBS) -o $@


tidy:
	$(POAC_TIDY) $(SRCS) -- $(CXXFLAGS) $(DEFINES) -DPOAC_TEST $(INCLUDES)

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
