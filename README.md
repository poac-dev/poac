<div align="center"><img alt="poac" width="250" src="https://github.com/poacpm/design/raw/main/images/logo.svg"></div>

## Description

Poac (pronounced as `/pəʊək/`) is a package manager for C++ users.

Poac can download project's dependencies and compile a project.
Please see [poac.pm](https://poac.pm), [installation instructions](https://doc.poac.pm/en/getting-started/installation.html), and [The Poac Book](https://doc.poac.pm) for more details.

:warning: Caution! Currently in development and cannot be used.

## Demo

By using Poac, you can create a C++ project, build sources, and execute an application:

![Poac Demo](https://user-images.githubusercontent.com/26405363/100546063-9b81d500-32a2-11eb-9018-01f05e8d9252.gif)

## Supported Operating Systems

|                                                                                   Linux                                                                                    |                                                                                   macOS                                                                                    |
|:--------------------------------------------------------------------------------------------------------------------------------------------------------------------------:|:--------------------------------------------------------------------------------------------------------------------------------------------------------------------------:|
| [![GitHub Actions Linux Build](https://github.com/poacpm/poac/workflows/Linux/badge.svg?branch=main)](https://github.com/poacpm/poac/actions?query=workflow%3A%22Linux%22) | [![GitHub Actions macOS Build](https://github.com/poacpm/poac/workflows/macOS/badge.svg?branch=main)](https://github.com/poacpm/poac/actions?query=workflow%3A%22macOS%22) |

<!--
[![GitHub Actions Windows Build](https://github.com/poacpm/poac/workflows/Windows/badge.svg?branch=main)](https://github.com/poacpm/poac/actions?query=workflow%3A%22Windows%22)|
-->

Please see [1.1. Installation · The Poac Book](https://doc.poac.pm/en/getting-started/installation.html#supported-operating-systems) for more information about supported OS.

## Code Status

* GitHub:
[![GitHub Release Version](https://img.shields.io/github/release/poacpm/poac.svg?style=flat)](https://github.com/poacpm/poac/releases)
[![Github All Releases](https://img.shields.io/github/downloads/poacpm/poac/total.svg)](https://github.com/poacpm/poac/releases)
[![GitHub License](https://img.shields.io/github/license/poacpm/poac.svg)](https://github.com/awslabs/aws-c-common/blob/main/LICENSE)
[![FOSSA Status](https://app.fossa.io/api/projects/git%2Bgithub.com%2Fpoacpm%2Fpoac.svg?type=shield)](https://app.fossa.io/projects/git%2Bgithub.com%2Fpoacpm%2Fpoac?ref=badge_shield)

* Code Coverage:
[![Coverity Scan Build Status](https://scan.coverity.com/projects/17677/badge.svg)](https://scan.coverity.com/projects/poacpm-poac)
[![codecov](https://codecov.io/gh/poacpm/poac/branch/master/graph/badge.svg?token=eyNsQ5nugd)](https://codecov.io/gh/poacpm/poac)

* Code Quality:
[![Codacy Badge](https://app.codacy.com/project/badge/Grade/ac87f6b4a0284a2d8b88f3feb6c19f2b)](https://www.codacy.com/gh/poacpm/poac/dashboard?utm_source=github.com&amp;utm_medium=referral&amp;utm_content=poacpm/poac&amp;utm_campaign=Badge_Grade)
[![Language grade: JavaScript](https://img.shields.io/lgtm/grade/javascript/g/poacpm/poac.svg?logo=lgtm&logoWidth=18)](https://lgtm.com/projects/g/poacpm/poac/context:javascript)
[![CodeFactor](https://www.codefactor.io/repository/github/poacpm/poac/badge)](https://www.codefactor.io/repository/github/poacpm/poac)

## Installation

[![Packaging status](https://repology.org/badge/vertical-allrepos/poac.svg)](https://repology.org/project/poac/versions)

### Easy install

```bash
curl -fsSL https://sh.poac.pm | bash
```

*For Arch Linux users, there are AUR packages: [poac](https://aur.archlinux.org/packages/poac/), [poac-devel-git](https://aur.archlinux.org/packages/poac-devel-git), and [poac-git](https://aur.archlinux.org/packages/poac-git)*

### Manual install (Build)

Poac requires the following compilers, tools, and packages to build:

#### compilers

* Compilers which support [C++20](https://en.cppreference.com/w/cpp/20)
  * `gcc`: `11` or later
  * `clang`: `12` or later
  * `Apple Clang`: provided by `macOS Big Sur` or later

#### tools

* [`cmake`](https://gitlab.kitware.com/cmake/cmake): [`3.21`](https://gitlab.kitware.com/cmake/cmake/-/tree/v3.21.6) or later
* [`ninja`](https://github.com/ninja-build/ninja): [`1.8`](https://github.com/ninja-build/ninja/releases/tag/v1.8.2) or later

#### libraries

* [`boost`](https://github.com/boostorg): [`1.70.0`](https://github.com/boostorg/boost/releases/tag/boost-1.70.0) or later
  * algorithm
  * asio
  * beast
  * dynamic_bitset
  * graph
  * predef
  * property_tree
  * range
  * regex
  * scope_exit
  * uuid
* [`openssl`](https://github.com/openssl/openssl): as new as possible

<details>
<summary>The following libraries will be automatically installed when configuring with CMake, so generally, you do not need to care about them.</summary>

**dependencies**

* [`fmt`](https://github.com/fmtlib/fmt): [`7.1.3`](https://github.com/fmtlib/fmt/releases/tag/7.1.3) or later
* [`git2-cpp`](https://github.com/ken-matsui/git2-cpp): [`v0.1.0-alpha.0`](https://github.com/ken-matsui/git2-cpp/releases/tag/v0.1.0-alpha.0) or later
* [`libarchive`](https://github.com/libarchive/libarchive): [`master`](https://github.com/libarchive/libarchive/tree/master) branch
  * requires [this commit](https://github.com/libarchive/libarchive/commit/a4c3c90bb828ab5f01589718266ac5d3fdccb854)
  * awaiting the next release above [`v3.6.0`](https://github.com/libarchive/libarchive/releases/tag/v3.6.0)
* [`libgit2`](https://github.com/libgit2/libgit2): [`0.27`](https://github.com/libgit2/libgit2/releases/tag/v0.27.7) or later
* [`mitama-cpp-result`](https://github.com/LoliGothick/mitama-cpp-result): [`master`](https://github.com/LoliGothick/mitama-cpp-result/tree/master) branch
  * requires [this commit](https://github.com/LoliGothick/mitama-cpp-result/commit/715174c94c48a96221b9d74a8b3489c8eea140e9)
  * awaiting the next release above [`v9.2.1`](https://github.com/LoliGothick/mitama-cpp-result/releases/tag/v9.2.1)
* [`ninja`](https://github.com/ninja-build/ninja): [`v1.10.2`](https://github.com/ninja-build/ninja/releases/tag/v1.10.2) or later
* [`spdlog`](https://github.com/gabime/spdlog): [`1.9.0`](https://github.com/gabime/spdlog/releases/tag/v1.9.0) or later
* [`structopt`](https://github.com/p-ranav/structopt): [`v0.1.2`](https://github.com/p-ranav/structopt/releases/tag/v0.1.2) or later
* [`toml11`](https://github.com/ToruNiina/toml11): [`v3.7.1`](https://github.com/ToruNiina/toml11/releases/tag/v3.7.1) branch
  * requires [this commit](https://github.com/ToruNiina/toml11/commit/21732fce459911d4da0214baff52af7c8c48cc9f)

**dev-dependencies**

* [`μt`](https://github.com/boost-ext/ut): [`v1.1.9`](https://github.com/boost-ext/ut/releases/tag/v1.1.9) or later
</details>

After you prepared these requirements, you can build Poac using the following commands:

```bash
$ git clone https://github.com/poacpm/poac.git
$ cd poac
$ mkdir build && cd $_
$ cmake .. -GNinja -DCMAKE_BUILD_TYPE=Release
$ ninja
$ ninja install
```

## Why Poac?

C++ is often considered to be a complicated language and shunned unconsciously by most people.
It is thought that it is hard to construct a C++ environment, no definitive package manager, and the strange syntax of build systems such as [CMake](https://cmake.org) make us feel hesitant.

By developing a package manager and build system, which have an intuitively easy-to-use interface like [npm](https://www.npmjs.com) and [Cargo](https://github.com/rust-lang/cargo) and make users be able to develop applications and libraries without being aware of [CMake](https://cmake.org), developers will be able to focus on learning C++ without stumbling.
I also plan to implement integration with many other build systems and package managers so that you can seamlessly switch a development environment.

## Contributing

Please see [CONTRIBUTING.md](https://github.com/poacpm/.github/blob/main/CONTRIBUTING.md).
You can also find the useful [architecture documentation](https://doc.poac.pm/en/architecture.html).

This project exists thanks to all the people who contribute.

<a href="https://github.com/poacpm/poac/graphs/contributors">
  <img src="https://contributors-img.web.app/image?repo=poacpm/poac" />
</a>

## License

Poac is licensed under the terms of the Apache License version 2.0.

Please see [LICENSE](LICENSE) for details.

[![FOSSA Status](https://app.fossa.io/api/projects/git%2Bgithub.com%2Fpoacpm%2Fpoac.svg?type=large)](https://app.fossa.io/projects/git%2Bgithub.com%2Fpoacpm%2Fpoac?ref=badge_large)

### Third-party software

* boost - <https://github.com/boostorg/boost/blob/master/LICENSE_1_0.txt>
* fmt - <https://github.com/fmtlib/fmt/blob/master/LICENSE.rst>
* git2-cpp - <https://github.com/ken-matsui/git2-cpp/blob/main/LICENSE>
* libarchive - <https://github.com/libarchive/libarchive/blob/master/COPYING>
* libgit2 - <https://github.com/libgit2/libgit2/blob/master/COPYING>
* mitama-cpp-result - <https://github.com/LoliGothick/mitama-cpp-result/blob/master/LICENSE>
* ninja - <https://github.com/ninja-build/ninja/blob/master/COPYING>
* openssl - <https://github.com/openssl/openssl/blob/master/LICENSE.txt>
* spdlog - <https://github.com/gabime/spdlog/blob/v1.x/LICENSE>
* structopt - <https://github.com/p-ranav/structopt/blob/master/LICENSE>
* toml11 - <https://github.com/ToruNiina/toml11/blob/master/LICENSE>
* μt - <https://github.com/boost-ext/ut/blob/master/LICENSE.md>
