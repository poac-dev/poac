<div align="center">
  <img alt="logo-white" width="250" src="https://user-images.githubusercontent.com/26405363/170157214-51b03bbd-cd29-432b-99ca-8f65c01f973e.svg#gh-dark-mode-only">
  <img alt="logo-black" width="250" src="https://user-images.githubusercontent.com/26405363/170157184-e338188c-e4fa-4967-ac74-aba03aefa0bc.svg#gh-light-mode-only">
</div>

## Description

Poac (pronounced as `/pəʊək/`) is a package manager for C++ users.

Poac can download project's dependencies and compile a project.
Please visit [poac.pm](https://poac.pm) and [The Poac Book](https://doc.poac.pm) for more details.

## Demo

By using Poac, you can create a C++ project, build sources, and execute an application:

![demo](https://user-images.githubusercontent.com/26405363/169694620-2e3f769e-c6c5-4e6a-a645-e5bae226a8cb.gif)

## Usage

### Start a new project with Poac

Use this command when you start a new poac project.

```bash
$ poac create hello_world
     Created binary (application) `hello_world` package
```

> If you want to integrate your existing project with Poac, use the `init` command:
> 
> ```bash
> your-pj/$ poac init
>      Created binary (application) `your-pj` package
> ```
> 
> This command just creates a `poac.toml` file not to let your project break.

### Build the project

In most cases, you will want to execute as well as build—of course, you can.

```bash
hello_world/$ poac run
   Compiling 1/1: hello_world v0.1.0 (/Users/ken-matsui/hello_world)
    Finished debug target(s) in 0.90s
     Running `/Users/ken-matsui/hello_world/poac_output/debug/hoge`
Hello, world!
```

Should you just build it, run the `build` command:

```bash
hello_world/$ poac build
    Finished debug target(s) in 0.21s
```

Poac uses a cache since we executed the command with no changes.

### Install dependencies

Like Cargo for Rust does, Poac installs dependencies at build time.
However, Poac does not support [weired specifiers](https://stackoverflow.com/q/22343224) for versions, such as `~` and `^`.
You can specify dependencies like:

`poac.toml`

```toml
[dependencies]
"boost/bind" = ">=1.64.0 and <2.0.0"
```

We regularly avoid auto updating packages to major versions which bring breaking changes, but minor and patch are acceptable.

> If you would use a specific version, you can write the version as following:
>
> ```toml
> [dependencies]
> "boost/bind" = "1.66.0"
> ```

After editing `poac.toml`, executing the `build` command will install the package and its dependencies.

```bash
hello_world/$ poac build
 Downloading packages ...
  Downloaded boost/bind v1.66.0
  Downloaded boost/core v1.66.0
  Downloaded boost/assert v1.66.0
  Downloaded boost/config v1.66.0
   Compiling 1/1: hello_world v0.1.0 (/Users/ken-matsui/hello_world)
    Finished debug target(s) in 0.70s
```

To use this dependency, update the `main.cpp` file.

`src/main.cpp`

```cpp
#include <iostream>
#include <boost/bind.hpp>

int f(int a, int b) {
  return a + b;
}

int main(int argc, char** argv) {
  std::cout << boost::bind(f, 5, _1)(10) << std::endl;
}
```

You can now run this source code:

```bash
hello_world/$ poac run
   Compiling 1/1: hello_world v0.1.0 (/Users/ken-matsui/hello_world)
    Finished debug target(s) in 0.50s
     Running `/Users/ken-matsui/hello_world/poac_output/debug/hello_world`
15
```

> We currently support building a project with header-only dependencies.
> Building with build-required dependencies will be soon supported.

### Run linter

Linting source code is essential to protect its quality.
Poac supports linting it by a simple command with `cpplint`:

```bash
hello_world/$ poac lint
     Linting poac
src/main.cpp:0:  No copyright message found.  You should have a line: "Copyright [year] <Copyright Owner>"  [legal/copyright] [5]
Done processing src/main.cpp
Total errors found: 1

Error: `cpplint` completed with exit code 1
```

> If you do not have `cpplint`, install it with the following command:
>
> ```bash
> $ pip install cpplint
> ```

The `lint` command works without configurations, and Poac would automatically opt out of unwanted lints by adjusting to each project.
To customize the lint settings, try creating a [`CPPLINT.cfg`](/CPPLINT.cfg) file to the repository root.

### Run formatter

Poac also supports formatting your source code with `clang-format`.
Ensure having installed `clang-format` before running this command.

```bash
$ poac fmt
  Formatting poac
```

> This command automatically detects what files we need to format to avoid bothering commands like:
>
> ```bash
> $ # We need to avoid the `build` dir and such dirs ...
> $ clang-format ./src/*.cpp -i
> $ clang-format ./include/**/*.hpp -i
> $ clang-format ./tests/**/*.cpp -i
> $ ...
> ```

To customize the format settings, try creating a [`.clang-format`](/.clang-format) file to the repository root.

### Search packages

In case you would find what packages are provided, you can use the `search` command or visit [poac.pm](https://poac.pm).

```bash
$ poac search func
boost/function = "1.66.0"               # Boost.org function module
boost/function_types = "1.66.0"         # Boost.org function_types module
boost/functional = "1.66.0"             # Boost.org functional module
```

### Publish packages

WIP

## Roadmap

Poac is still under development and may contain a bunch of bugs.

|                        Feature                        |       Status       |
|:-----------------------------------------------------:|:------------------:|
|                 Install dependencies                  | :white_check_mark: |
|     Build packages with header-only dependencies      | :white_check_mark: |
|               Support dev-dependencies                | :white_check_mark: |
| Build packages separated by headers & implementations |                    |
|                   Publish packages                    |        WIP         |
|    Build packages with build-required dependencies    |                    |
|               Build packages with CMake               |                    |

## Supported Operating Systems

|                                                                                   Linux                                                                                    |                                                                                   macOS                                                                                    |
|:--------------------------------------------------------------------------------------------------------------------------------------------------------------------------:|:--------------------------------------------------------------------------------------------------------------------------------------------------------------------------:|
| [![GitHub Actions Linux Build](https://github.com/poacpm/poac/workflows/Linux/badge.svg?branch=main)](https://github.com/poacpm/poac/actions?query=workflow%3A%22Linux%22) | [![GitHub Actions macOS Build](https://github.com/poacpm/poac/workflows/macOS/badge.svg?branch=main)](https://github.com/poacpm/poac/actions?query=workflow%3A%22macOS%22) |

## Code Status

* GitHub:
[![GitHub Release Version](https://img.shields.io/github/release/poacpm/poac.svg?style=flat)](https://github.com/poacpm/poac/releases)
[![Github All Releases](https://img.shields.io/github/downloads/poacpm/poac/total.svg)](https://github.com/poacpm/poac/releases)
[![GitHub License](https://img.shields.io/github/license/poacpm/poac.svg)](https://github.com/awslabs/aws-c-common/blob/main/LICENSE)
[![FOSSA Status](https://app.fossa.io/api/projects/git%2Bgithub.com%2Fpoacpm%2Fpoac.svg?type=shield)](https://app.fossa.io/projects/git%2Bgithub.com%2Fpoacpm%2Fpoac?ref=badge_shield)

* Code Coverage:
[![codecov](https://codecov.io/gh/poacpm/poac/branch/master/graph/badge.svg?token=eyNsQ5nugd)](https://codecov.io/gh/poacpm/poac)

* Code Quality:
[![Codacy Badge](https://app.codacy.com/project/badge/Grade/ac87f6b4a0284a2d8b88f3feb6c19f2b)](https://www.codacy.com/gh/poacpm/poac/dashboard?utm_source=github.com&amp;utm_medium=referral&amp;utm_content=poacpm/poac&amp;utm_campaign=Badge_Grade)
[![CodeFactor](https://www.codefactor.io/repository/github/poacpm/poac/badge)](https://www.codefactor.io/repository/github/poacpm/poac)

## Installation

Since all packages through these providers may not be maintained by Poac owners, install them at your own risk.

[![Packaging status](https://repology.org/badge/vertical-allrepos/poac.svg)](https://repology.org/project/poac/versions)

### macOS

```bash
brew install poacpm/tap/poac
```

### Arch Linux

> [poac](https://aur.archlinux.org/packages/poac/), [poac-devel-git](https://aur.archlinux.org/packages/poac-devel-git), and [poac-git](https://aur.archlinux.org/packages/poac-git)

```bash
pacman -S poac
```

### Build from source

Should your environment is not listed on released packages, you will need to build Poac from source.
Poac requires the following compilers, tools, and libraries to build:

#### compilers

* Compilers which support [C++20](https://en.cppreference.com/w/cpp/20)
  * `GCC`: `11` or later
  * `Clang`: `12` or later
  * `Apple Clang`: provided by `macOS Big Sur` or later

#### tools

* [`CMake`](https://gitlab.kitware.com/cmake/cmake): [`3.21`](https://gitlab.kitware.com/cmake/cmake/-/tree/v3.21.6) or later
* [`Ninja`](https://github.com/ninja-build/ninja): [`1.8`](https://github.com/ninja-build/ninja/releases/tag/v1.8.2) or later

#### libraries

* [`boost`](https://github.com/boostorg): [`1.70.0`](https://github.com/boostorg/boost/releases/tag/boost-1.70.0) or later
  * algorithm
  * asio
  * beast
  * container_hash
  * dynamic_bitset
  * graph
  * predef
  * preprocessor
  * property_tree
  * range
  * regex
  * scope_exit
  * uuid
* [`openssl`](https://github.com/openssl/openssl): [`3.0.0`](https://github.com/openssl/openssl/releases/tag/openssl-3.0.0) or later
  * some `SHA256` functions are marked as [deprecated](https://github.com/openssl/openssl/blob/openssl-3.0.0/include/openssl/sha.h#L57-L79) since `3.0.0`

<details>
<summary>The following libraries will be automatically installed when configuring with CMake, so generally, you do not need to care about them.</summary>

---

**dependencies**

* [`fmt`](https://github.com/fmtlib/fmt): [`7.1.3`](https://github.com/fmtlib/fmt/releases/tag/7.1.3) or later
* [`git2-cpp`](https://github.com/ken-matsui/git2-cpp): [`v0.1.0-alpha.1`](https://github.com/ken-matsui/git2-cpp/releases/tag/v0.1.0-alpha.1) or later
* [`glob`](https://github.com/p-ranav/glob): [`v0.0.1`](https://github.com/p-ranav/glob/releases/tag/v0.0.1) or later
* [`libarchive`](https://github.com/libarchive/libarchive): [`v3.6.1`](https://github.com/libarchive/libarchive/tree/master) or later
  * requires [this commit](https://github.com/libarchive/libarchive/commit/a4c3c90bb828ab5f01589718266ac5d3fdccb854)
* [`libgit2`](https://github.com/libgit2/libgit2): [`0.27`](https://github.com/libgit2/libgit2/releases/tag/v0.27.7) or later
* [`mitama-cpp-result`](https://github.com/LoliGothick/mitama-cpp-result): [`v9.3.0`](https://github.com/LoliGothick/mitama-cpp-result/releases/tag/v9.3.0) or later
  * requires [this commit](https://github.com/LoliGothick/mitama-cpp-result/commit/ec7f22ae921f750b0115681623d0c06223737819)
* [`ninja`](https://github.com/ninja-build/ninja): [`master`](https://github.com/ninja-build/ninja/tree/master) branch
  * requires [`src/status.h`](https://github.com/ninja-build/ninja/blob/ad3d29fb5375c3122b2318ea5efad170b83e74e5/src/status.h)
  * awaiting the next release above [`v1.10.2`](https://github.com/ninja-build/ninja/releases/tag/v1.10.2)
* [`spdlog`](https://github.com/gabime/spdlog): [`1.9.0`](https://github.com/gabime/spdlog/releases/tag/v1.9.0) or later
* [`structopt`](https://github.com/p-ranav/structopt): [`e9722d3`](https://github.com/p-ranav/structopt/commit/e9722d3c2b52cf751ebc1911b93d9649c4e365cc) or later
  * requires [this commit](https://github.com/p-ranav/structopt/commit/e9722d3c2b52cf751ebc1911b93d9649c4e365cc)
  * awaiting the next release above [`v0.1.3`](https://github.com/p-ranav/structopt/releases/tag/v0.1.3)
* [`toml11`](https://github.com/ToruNiina/toml11): [`c26aa01`](https://github.com/ToruNiina/toml11/commit/c26aa013cdc75286f90e6d9f661c14890b3f358f) or later
  * requires [this commit](https://github.com/ToruNiina/toml11/commit/c26aa013cdc75286f90e6d9f661c14890b3f358f)
  * awaiting the next release above [`v3.7.1`](https://github.com/ToruNiina/toml11/releases/tag/v3.7.1)

**dev-dependencies**

* [`μt`](https://github.com/boost-ext/ut): [`v1.1.9`](https://github.com/boost-ext/ut/releases/tag/v1.1.9) or later

---

</details>

After you prepared these requirements, you can build Poac using the following commands:

```bash
$ git clone https://github.com/poacpm/poac.git
$ cd poac
$ cmake -B build -G Ninja -DCMAKE_BUILD_TYPE=Release
$ cd build
$ ninja
$ ninja install
```

## Why Poac?

C++ is often considered to be a complicated language and shunned unconsciously by most people.
It is thought that it is hard to construct a C++ environment, no definitive package manager, and the strange syntax of build systems such as [CMake](https://cmake.org) make us feel hesitant.

By developing a package manager and build system, which have an intuitively easy-to-use interface like [npm](https://www.npmjs.com) and [Cargo](https://github.com/rust-lang/cargo) and make users be able to develop applications and libraries without being aware of [CMake](https://cmake.org), developers will be able to focus on learning C++ without stumbling.
I also plan to implement integration with many other build systems and package managers so that you can seamlessly switch a development environment.

### Naming Background

Poac is originated from `cpp` but also designed to emphasize ease of typing and avoiding the burden being placed on only one hand as Poac will be entered many times as a command.
The ergonomically optimized name prevents you from leading to tenosynovitis.

<img width="1230" alt="ergo" src="https://user-images.githubusercontent.com/26405363/169741684-ecb8aaad-2599-45ae-a680-a26f11652132.png">

As I mentioned before, C++ is often avoided being selected for product development; however, I would like to disseminate C++ as a fun language through Poac.
Amemiya and Mizutani argue that the `/p/` sound gives the brightest and softest impression among Japanese consonants (157).[^1]
Accordingly, I believe Poac would likewise provide a bright and soft impression.

## Contributing

### Linter

```bash
$ cpplint --quiet --recursive .
```

> If you have installed Poac, you can just run the `lint` command:
>
> ```bash
> $ poac lint
> ```

### Formatter

```bash
$ clang-format ./include/**/*.hpp -i
```

> If you have installed Poac, you can just run the `fmt` command:
>
> ```bash
> $ poac fmt
> ```

These are also done when pre-pushing changes in [`.githooks/pre-push`](/.githooks/pre-push).
In case you would skip these hooks, use the [`--no-verify`](https://stackoverflow.com/a/7230886) option.

Please see [CONTRIBUTING.md](https://github.com/poacpm/.github/blob/main/CONTRIBUTING.md) for more details.

---

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
* glob - <https://github.com/p-ranav/glob/blob/master/LICENSE>
* libarchive - <https://github.com/libarchive/libarchive/blob/master/COPYING>
* libgit2 - <https://github.com/libgit2/libgit2/blob/master/COPYING>
* mitama-cpp-result - <https://github.com/LoliGothick/mitama-cpp-result/blob/master/LICENSE>
* ninja - <https://github.com/ninja-build/ninja/blob/master/COPYING>
* openssl - <https://github.com/openssl/openssl/blob/master/LICENSE.txt>
* spdlog - <https://github.com/gabime/spdlog/blob/v1.x/LICENSE>
* structopt - <https://github.com/p-ranav/structopt/blob/master/LICENSE>
* toml11 - <https://github.com/ToruNiina/toml11/blob/master/LICENSE>
* μt - <https://github.com/boost-ext/ut/blob/master/LICENSE.md>


[^1]: Amemiya, T., & Mizutani, S. (2006). On the Basic Affective Dimensions of Japanese Onomatopoeia and the Basic Level of Japanese Phonesthemes. 関西大学社会学部紀要, 37(2), 139–166. https://hdl.handle.net/10112/12311
