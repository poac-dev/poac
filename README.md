<img alt="Terraform" src="https://raw.githubusercontent.com/poacpm/designs/master/images/logo.png" width="200px">


Poac is the package manager for C++ user.

Poac can downloads project's dependencies and compiles project.

Please see [poac.pm](https://poac.io) for [installation instructions](https://poacpm.github.io/poac/en/getting-started/installation.html) and [other documentation](https://docs.poac.io).


[![asciicast](https://asciinema.org/a/QwgRXsyeMYk62vwuZ6X6DZvcC.png)](https://asciinema.org/a/QwgRXsyeMYk62vwuZ6X6DZvcC)

## Supported Operating Systems
| Linux (= x86_64 GNU/Linux) | macOS (>= sierra) | Windows (= Windows Subsystem for Linux) |
|:---:|:---:|:---:|
|[![CircleCI](https://circleci.com/gh/poacpm/poac.svg?style=shield)](https://circleci.com/gh/poacpm/poac)|[![Travis CI](https://travis-ci.com/poacpm/poac.svg?branch=master)](https://travis-ci.com/poacpm/poac)|[![Build status](https://ci.appveyor.com/api/projects/status/6r7d0526he3nsq7l?svg=true)](https://ci.appveyor.com/project/matken11235/poac)|

## Code Status
[![GitHub](https://img.shields.io/github/license/poacpm/poac.svg)](https://github.com/awslabs/aws-c-common/blob/master/LICENSE)
[![Language grade: JavaScript](https://img.shields.io/lgtm/grade/javascript/g/poacpm/poac.svg?logo=lgtm&logoWidth=18)](https://lgtm.com/projects/g/poacpm/poac/context:javascript)
[![Coverage Status](https://coveralls.io/repos/github/poacpm/poac/badge.svg?branch=master)](https://coveralls.io/github/poacpm/poac?branch=master)

## Installation
### Easy install
```bash
curl -fsSL https://sh.poac.io | bash
```
*When your OS is macOS, use [Homebrew](https://github.com/Homebrew/brew)*

### Manual install (Build)
Poac requires the following tools and packages to build:
* [`boost`](https://github.com/boostorg): `1.66.0` or higher
* [`cmake`](https://github.com/Kitware/CMake): `3.0` or higher
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

**For Cygwin users:** Currently, you need to pass some options to `cmake` like below (for more detail, see [#93](https://github.com/poacpm/poac/issues/93)):

```bash
cmake .. -DCMAKE_CXX_FLAGS="-D_GNU_SOURCE -DBOOST_ASIO_HAS_STD_STRING_VIEW -Wa,-mbig-obj"
```

<!--
Already poac is installed, you can build using poac:
```bash
$ poac build
```
-->

## Requirements (runtime)
* compiler (gcc | clang | MSVC | ICC)
* `tar`: in publish command
* `dot(graphviz)`: in graph command
* `cmake`: optional
* `make`: optional

<!--
## Contribution
Please see at [CONTRIBUTING.md](.github/CONTRIBUTING.md)
-->