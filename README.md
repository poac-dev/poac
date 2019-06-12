<img alt="poac" src="https://raw.githubusercontent.com/poacpm/designs/master/images/logo.png" width="200px">


Poac is a package manager for C++ users.

Poac can download project's dependencies and compile project.

Please see [poac.pm](https://poac.pm) for [installation instructions](https://doc.poac.pm/en/getting-started/installation.html) and [other documentations](https://doc.poac.pm).


[![asciicast](https://asciinema.org/a/QwgRXsyeMYk62vwuZ6X6DZvcC.png)](https://asciinema.org/a/QwgRXsyeMYk62vwuZ6X6DZvcC)

## Supported Operating Systems
| Linux | macOS | Windows |
|:---:|:---:|:---:|
|[![CircleCI](https://circleci.com/gh/poacpm/poac.svg?style=shield)](https://circleci.com/gh/poacpm/poac)|[![Travis CI](https://travis-ci.com/poacpm/poac.svg?branch=master)](https://travis-ci.com/poacpm/poac)|[![Build status](https://ci.appveyor.com/api/projects/status/6r7d0526he3nsq7l/branch/master?svg=true)](https://ci.appveyor.com/project/matken11235/poac/branch/master)|

Please see [1.1. Installation · The Poac Book](https://doc.poac.pm/en/getting-started/installation.html#supported-operating-systems) for more information about supported OS.

## Code Status
[![GitHub License](https://img.shields.io/github/license/poacpm/poac.svg)](https://github.com/awslabs/aws-c-common/blob/master/LICENSE)
[![Github All Releases](https://img.shields.io/github/downloads/poacpm/poac/total.svg)](https://github.com/poacpm/poac/releases)
[![Coverity Scan Build Status](https://scan.coverity.com/projects/17677/badge.svg)](https://scan.coverity.com/projects/poacpm-poac)
[![Coverage Status](https://coveralls.io/repos/github/poacpm/poac/badge.svg?branch=master)](https://coveralls.io/github/poacpm/poac?branch=master)
[![codecov](https://codecov.io/gh/poacpm/poac/branch/master/graph/badge.svg)](https://codecov.io/gh/poacpm/poac)
[![Codacy Badge](https://api.codacy.com/project/badge/Grade/4179a24c6e514bc0b3344f80bf64a40d)](https://app.codacy.com/app/matken11235/poac?utm_source=github.com&utm_medium=referral&utm_content=poacpm/poac&utm_campaign=Badge_Grade_Settings)
[![Language grade: JavaScript](https://img.shields.io/lgtm/grade/javascript/g/poacpm/poac.svg?logo=lgtm&logoWidth=18)](https://lgtm.com/projects/g/poacpm/poac/context:javascript)
[![CodeFactor](https://www.codefactor.io/repository/github/poacpm/poac/badge)](https://www.codefactor.io/repository/github/poacpm/poac)

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

<!--
If poac is already installed, you can build using poac:
```bash
$ poac build
```
-->

## Runtime requirements
* compiler (gcc | clang | MSVC | ICC)
* `tar`: in publish command
* `dot(graphviz)`: in graph command
* `git`: in install command
* `cmake`: optional
* `make`: optional

<!--
## Contribution
Please see [CONTRIBUTING.md](.github/CONTRIBUTING.md)
-->
