<div align="center"><img alt="poac" width="250" src="https://github.com/poacpm/design/raw/main/images/logo.svg"></div>

## Description

Poac is a package manager for C++ users.

Poac can download project's dependencies and compile project.
Please see [poac.pm](https://poac.pm), [installation instructions](https://doc.poac.pm/en/getting-started/installation.html) and [The Poac Book](https://doc.poac.pm) for more details.

:warning: Caution! Currently in development and cannot be used.

## Demo

By using Poac, you can create a C++ project, build sources and execute an application:

![Poac Demo](https://user-images.githubusercontent.com/26405363/100546063-9b81d500-32a2-11eb-9018-01f05e8d9252.gif)

## Supported Operating Systems

| Linux | macOS |
|:-----:|:-----:|
|[![GitHub Actions Linux Build](https://github.com/poacpm/poac/workflows/Linux/badge.svg?branch=main)](https://github.com/poacpm/poac/actions?query=workflow%3A%22Linux%22)|[![GitHub Actions macOS Build](https://github.com/poacpm/poac/workflows/macOS/badge.svg?branch=main)](https://github.com/poacpm/poac/actions?query=workflow%3A%22macOS%22)|

<!--
 Windows |
:-------:|
[![GitHub Actions Windows Build](https://github.com/poacpm/poac/workflows/Windows/badge.svg?branch=main)](https://github.com/poacpm/poac/actions?query=workflow%3A%22Windows%22)|
-->

<!-- [![Appveyor build status](https://ci.appveyor.com/api/projects/status/6r7d0526he3nsq7l/branch/main?svg=true)](https://ci.appveyor.com/project/matken11235/poac/branch/main) -->

Please see [1.1. Installation · The Poac Book](https://doc.poac.pm/en/getting-started/installation.html#supported-operating-systems) for more information about supported OS.

## Code Status

* GitHub:
[![GitHub Release Version](https://img.shields.io/github/release/poacpm/poac.svg?style=flat)](https://github.com/poacpm/poac/releases)
[![Github All Releases](https://img.shields.io/github/downloads/poacpm/poac/total.svg)](https://github.com/poacpm/poac/releases)
[![GitHub License](https://img.shields.io/github/license/poacpm/poac.svg)](https://github.com/awslabs/aws-c-common/blob/main/LICENSE)
[![FOSSA Status](https://app.fossa.io/api/projects/git%2Bgithub.com%2Fpoacpm%2Fpoac.svg?type=shield)](https://app.fossa.io/projects/git%2Bgithub.com%2Fpoacpm%2Fpoac?ref=badge_shield)

* Code Coverage:
[![Coverity Scan Build Status](https://scan.coverity.com/projects/17677/badge.svg)](https://scan.coverity.com/projects/poacpm-poac)
[![codecov](https://codecov.io/gh/poacpm/poac/branch/main/graph/badge.svg)](https://codecov.io/gh/poacpm/poac)

* Code Quality:
[![Codacy Badge](https://api.codacy.com/project/badge/Grade/4179a24c6e514bc0b3344f80bf64a40d)](https://app.codacy.com/app/matken11235/poac?utm_source=github.com&utm_medium=referral&utm_content=poacpm/poac&utm_campaign=Badge_Grade_Settings)
[![Language grade: JavaScript](https://img.shields.io/lgtm/grade/javascript/g/poacpm/poac.svg?logo=lgtm&logoWidth=18)](https://lgtm.com/projects/g/poacpm/poac/context:javascript)
[![CodeFactor](https://www.codefactor.io/repository/github/poacpm/poac/badge)](https://www.codefactor.io/repository/github/poacpm/poac)

## Installation

[![Packaging status](https://repology.org/badge/vertical-allrepos/poac.svg)](https://repology.org/project/poac/versions)

### Easy install

```bash
curl -fsSL https://sh.poac.pm | bash
```

*For Arch Linux users, there are AUR packages: [poac](https://aur.archlinux.org/packages/poac/), [poac-devel-git](https://aur.archlinux.org/packages/poac-devel-git), [poac-git](https://aur.archlinux.org/packages/poac-git)*

### Manual install (Build)

Poac requires the following compilers, tools, and packages to build:

#### compilers

* Compilers which support [C++20](https://en.cppreference.com/w/cpp/20)

#### tools

* [`cmake`](https://github.com/Kitware/CMake): `3.17` or later

#### packages

> The packages with names in italics are not needed installing before the following commands
> because they will be automatically installed when configuring by CMake.

* [`boost`](https://github.com/boostorg): `1.70.0` or later
  * algorithm
  * asio
  * beast
  * dynamic_bitset
  * graph
  * predef
  * property_tree
  * range
  * scope_exit
  * test (dev)
  * uuid
* [_`clipp`_](https://github.com/muellan/clipp): `master` branch
* [_`fmt`_](https://github.com/fmtlib/fmt): `7.1.3` or later
* [_`git2-cpp`_](https://github.com/ken-matsui/git2-cpp): `main` branch
* [`libarchive`](https://github.com/libarchive/libarchive): `3.4.3` or later
* [_`libgit2`_](https://github.com/libgit2/libgit2): `0.27` or later
* [_`mitama-cpp-result`_](https://github.com/LoliGothick/mitama-cpp-result): `master` branch
* [`openssl`](https://github.com/openssl/openssl): as new as possible
* [_`spdlog`_](https://github.com/gabime/spdlog): `1.9.0` or later
* [_`toml11`_](https://github.com/ToruNiina/toml11): `3.0.0` or later

After you prepared the requirements, you can build Poac using the following commands:

```bash
$ git clone https://github.com/poacpm/poac.git
$ cd poac
$ mkdir build && cd $_
$ cmake .. -DCMAKE_BUILD_TYPE=Release
$ make # or ninja
$ make install
```

## Why Poac?

I often see that C++ was said to be a difficult language, and I saw many people shunned it just because of C++.
It was thought that it is hard to construct an environment, there is no definitive package manager, and the strange syntax of the build system [CMake](https://cmake.org), etc. is the cause that makes us feel hesitant.

By developing a package manager and a build system that has an intuitive and easy-to-use interface like [npm](https://www.npmjs.com) and [Cargo](https://github.com/rust-lang/cargo), and users can develop applications and libraries without being aware of [CMake](https://cmake.org), you can focus on learning C++ without stumbling.
I also plan to implement integration with many other build systems and package managers, so you should be able to switch seamlessly.

## Contributing

Please see [CONTRIBUTING.md](.github/CONTRIBUTING.md).
You can also find the useful [architecture documentation](https://doc.poac.pm/en/architecture.html).

## License

Poac is licensed under the terms of the Apache License version 2.0.

Please see [LICENSE](https://github.com/poacpm/poac/blob/main/LICENSE) for details.

[![FOSSA Status](https://app.fossa.io/api/projects/git%2Bgithub.com%2Fpoacpm%2Fpoac.svg?type=large)](https://app.fossa.io/projects/git%2Bgithub.com%2Fpoacpm%2Fpoac?ref=badge_large)

### Third party software

* boost - <https://github.com/boostorg/boost/blob/master/LICENSE_1_0.txt>
* clipp - <https://github.com/muellan/clipp/blob/master/LICENSE>
* fmt - <https://github.com/fmtlib/fmt/blob/master/LICENSE.rst>
* git2-cpp - <https://github.com/ken-matsui/git2-cpp/blob/main/LICENSE>
* libarchive - <https://github.com/libarchive/libarchive/blob/master/COPYING>
* libgit2 - <https://github.com/libgit2/libgit2/blob/master/COPYING>
* mitama-cpp-result - <https://github.com/LoliGothick/mitama-cpp-result/blob/master/LICENSE>
* openssl - <https://github.com/openssl/openssl/blob/master/LICENSE.txt>
* spdlog - <https://github.com/gabime/spdlog/blob/v1.x/LICENSE>
* toml11 - <https://github.com/ToruNiina/toml11/blob/master/LICENSE>
