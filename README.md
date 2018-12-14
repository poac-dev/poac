**:warning: Not yet released**

# poac

Poac is the package manager for C/C++ user.

Poac can downloads project's dependencies and compiles project.

Please see [poac.pm](https://poac.pm) for [installation instructions](https://poacpm.github.io/poac/en/getting-started/installation.html) and [other documentation](https://poacpm.github.io/poac/).


[![asciicast](https://asciinema.org/a/QwgRXsyeMYk62vwuZ6X6DZvcC.png)](https://asciinema.org/a/QwgRXsyeMYk62vwuZ6X6DZvcC)

## Supported Operating Systems

Poac currently supports macOS (>= high sierra) only.


## Code Status
[![CircleCI](https://circleci.com/gh/poacpm/poac.svg?style=svg)](https://circleci.com/gh/poacpm/poac)
[![Build Status](https://travis-ci.org/poacpm/poac.svg?branch=master)](https://travis-ci.org/poacpm/poac)
[![Language grade: JavaScript](https://img.shields.io/lgtm/grade/javascript/g/poacpm/poac.svg?logo=lgtm&logoWidth=18)](https://lgtm.com/projects/g/poacpm/poac/context:javascript)


## Installation
### Easy install
```bash
curl -fsSL https://sh.poac.pm | bash
```
*When your OS is macOS, use [Homebrew](https://github.com/Homebrew/brew)*

### Manual install (Build)
Poac requires the following tools and packages to build:
* [`boost`](https://github.com/boostorg): `1.48.0` or higher
* [`cmake`](https://github.com/Kitware/CMake): `3.0` or higher
* [`libcurl`](https://github.com/curl/curl): `7.63.0`
* [`libressl`](https://www.libressl.org): `2.6.5`
* [`yaml-cpp`](https://github.com/jbeder/yaml-cpp): `0.6.0` or higher
* [`CVC4`](https://github.com/CVC4/CVC4): `1.6`

```bash
$ git clone https://github.com/poacpm/poac.git
$ cd poac
$ mkdir build && cd $_
$ cmake ..
$ make
$ make install
```

Already poac is installed, you can build using poac:
```bash
$ poac build
```


## Requirements (runtime)
* compiler (gcc | clang | MSVC | ICC)
* `tar`
* `git`
* `cmake`: optional
* `make`: optional


## Contribution
Please see at [CONTRIBUTUING.md](.github/CONTRIBUTUING.md).
