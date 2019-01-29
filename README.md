# poac

Poac is the package manager for C++ user.

Poac can downloads project's dependencies and compiles project.

Please see [poac.pm](https://poac.pm) for [installation instructions](https://poacpm.github.io/poac/en/getting-started/installation.html) and [other documentation](https://poacpm.github.io/poac/).


[![asciicast](https://asciinema.org/a/QwgRXsyeMYk62vwuZ6X6DZvcC.png)](https://asciinema.org/a/QwgRXsyeMYk62vwuZ6X6DZvcC)

## Supported Operating Systems
* macOS (>= sierra)
* Linux (= x86_64 GNU/Linux)

## Code Status
[![GitHub](https://img.shields.io/github/license/poacpm/poac.svg)](https://github.com/awslabs/aws-c-common/blob/master/LICENSE)
[![CircleCI](https://circleci.com/gh/poacpm/poac.svg?style=shield)](https://circleci.com/gh/poacpm/poac)
[![Travis CI](https://travis-ci.com/poacpm/poac.svg?branch=master)](https://travis-ci.com/poacpm/poac)
[![Language grade: JavaScript](https://img.shields.io/lgtm/grade/javascript/g/poacpm/poac.svg?logo=lgtm&logoWidth=18)](https://lgtm.com/projects/g/poacpm/poac/context:javascript)
[![Coverage Status](https://coveralls.io/repos/github/poacpm/poac/badge.svg?branch=master)](https://coveralls.io/github/poacpm/poac?branch=master)

## Installation
### Easy install
```bash
curl -fsSL https://sh.poac.pm | bash
```
*When your OS is macOS, use [Homebrew](https://github.com/Homebrew/brew)*

### Manual install (Build)
Poac requires the following tools and packages to build:
* [`boost`](https://github.com/boostorg): `1.66.0` or higher
* [`cmake`](https://github.com/Kitware/CMake): `3.0` or higher
* [`libcurl`](https://github.com/curl/curl): `7.63.0`
* [`openssl`](https://github.com/openssl/openssl): as new as possible
* [`yaml-cpp`](https://github.com/jbeder/yaml-cpp): `0.6.0` or higher

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
* `tar`: in publish command
* `cmake`: optional
* `make`: optional


## Contribution
Please see at [CONTRIBUTUING.md](.github/CONTRIBUTUING.md)
