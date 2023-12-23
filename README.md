<div align="center">
  <picture>
    <source media="(prefers-color-scheme: dark)" srcset="https://user-images.githubusercontent.com/26405363/170157214-51b03bbd-cd29-432b-99ca-8f65c01f973e.svg">
    <source media="(prefers-color-scheme: light)" srcset="https://user-images.githubusercontent.com/26405363/170157184-e338188c-e4fa-4967-ac74-aba03aefa0bc.svg">
    <img alt="Poac Logo" width="250" src="https://user-images.githubusercontent.com/26405363/170157184-e338188c-e4fa-4967-ac74-aba03aefa0bc.svg">
  </picture>
</div>

## Description

> [!IMPORTANT]
> Poac is still under development and may contain a bunch of bugs.

Poac (pronounced as `/pəʊək/`) is a package manager and build system for C++ users, inspired by Cargo for Rust.

Poac can download project's dependencies and compile a project.
Please visit [poac.dev](https://poac.dev) and [The Poac Book](https://doc.poac.dev) for more details.

## Demo

By using Poac, you can create a C++ project, build sources, and execute an application:

![demo](https://github.com/poac-dev/poac/assets/26405363/4486b424-e5a1-435e-a83e-fcd939bdaef0)

## Hello World

You can get started with just a few commands as the demo shows. Let's create a new Poac project:

```console
you:~$ poac new hello_world
     Created binary (application) `hello_world` package
```

Now, you can use the `poac run` command to run your application:

```console
you:~$ cd hello_world
you:~/hello_world$ poac run
 Compiling src/main.cc
   Linking hello_world
  Finished debug target(s) in 0.45386s
   Running poac-out/debug/hello_world
Hello, world!
```

## Supported Operating Systems

|                                                                      Linux                                                                       |                                                                      macOS                                                                       |
| :----------------------------------------------------------------------------------------------------------------------------------------------: | :----------------------------------------------------------------------------------------------------------------------------------------------: |
| [![Linux](https://github.com/poac-dev/poac/actions/workflows/linux.yml/badge.svg)](https://github.com/poac-dev/poac/actions/workflows/linux.yml) | [![macOS](https://github.com/poac-dev/poac/actions/workflows/macos.yml/badge.svg)](https://github.com/poac-dev/poac/actions/workflows/macos.yml) |

## Installation

### 3rd Party Installation (recommended)

Since packages through these providers may not be maintained by Poac owners, install them at your own risk.

[![Packaging status](https://repology.org/badge/vertical-allrepos/poac.svg)](https://repology.org/project/poac/versions)

It is important to verify the package name and copy it accurately to prevent typosquatting attacks. You can check the correct name by following the link in [`Packaging status` (Repology)](https://repology.org/project/poac/versions).

#### Homebrew (macOS & Linux)

```sh
brew install poac
```

#### MacPorts (macOS)

```sh
sudo port install poac
```

### Build from source

If your environment is not included in the released packages, you have to build Poac from the source. You will require the following compilers, tools, and libraries:

#### compilers

* Compilers which support [C++ 20](https://en.cppreference.com/w/cpp/20)
  * `GCC`: `11` or later
  * `Clang`: `15` or later
  * `Apple Clang`: provided by `macOS Big Sur (11)` or later

#### tools

* `Make`

#### libraries

When running Make, the following libraries will be installed automatically.

* [`toml11`](https://github.com/ToruNiina/toml11): [`9086b11`](https://github.com/ToruNiina/toml11/commit/9086b1114f39a8fb10d08ca704771c2f9f247d02) or later
  * requires [this commit](https://github.com/ToruNiina/toml11/commit/9086b1114f39a8fb10d08ca704771c2f9f247d02)
  * awaiting the next release above [`v3.7.1`](https://github.com/ToruNiina/toml11/releases/tag/v3.7.1)

Once you have all the necessary requirements in place, you can build Poac by the following commands:

```bash
git clone https://github.com/poac-dev/poac.git
cd poac
make RELEASE=1 install
```

## Usage

### Start a new project with Poac

The `poac new` command lets you start a new Poac project:

```console
you:~$ poac new hello_world
     Created binary (application) `hello_world` package
```

> [!TIP]
> If you want to integrate your existing project with Poac, use the `init` command:
>
> ```console
> you:~/your-pj$ poac init
>      Created binary (application) `your-pj` package
> ```
>
> This command just creates a `poac.toml` file not to break your project.

### Build the project

In most cases, you want to execute the generated binary as well as build the project.

```console
you:~/hello_world$ poac run
 Compiling src/main.cc
   Linking hello_world
  Finished debug target(s) in 0.45386s
   Running poac-out/debug/hello_world
Hello, world!
```

If you just want to build it, run the `build` command:

```console
you:~/hello_world$ poac build
  Finished debug target(s) in 0.00866317s
```

Poac uses a cache since we executed the command with no changes.

### Install dependencies

Like Cargo does, Poac installs dependencies at build time.  Poac currently supports only Git dependencies. You can specify dependencies like:

`poac.toml`

```toml
[dependencies]
"ToruNiina/toml11" = { git = "https://github.com/ToruNiina/toml11.git", rev = "846abd9a49082fe51440aa07005c360f13a67bbf" }
```

After editing `poac.toml`, executing the `build` command will install the package and its dependencies.

```console
you:~/hello_world$ poac build
Downloaded ToruNiina/toml11 846abd9a49082fe51440aa07005c360f13a67bbf
 Compiling src/main.cc
   Linking hello_world
  Finished debug target(s) in 0.70s
```

> [!WARNING]
> Poac currently supports building a project with header-only dependencies.
> Building with build-required dependencies will be soon supported.

### Run tests

You can write tests in any source files within the `src` directory.  Create files like:

`src/Lib.hpp`

```cpp
#ifndef HELLO_WORLD_LIB_HPP
#define HELLO_WORLD_LIB_HPP

int add(int, int);

#endif // HELLO_WORLD_LIB_HPP
```

`src/Lib.cc`

```cpp
#include "Lib.hpp"

int add(int a, int b) {
  return a + b;
}

#ifdef POAC_TEST

#  include <cassert>

int main() {
  assert(add(1, 2) == 3);
}

#endif
```

Now, with the `test` command, you can run tests defined within `POAC_TEST`:

```console
you:~/hello_world$ poac test
 Compiling src/Lib.cc
   Linking tests/test_Lib
   Testing Lib
  Finished debug test(s) in 0.565934s
```

### Run linter

Linting source code is essential to protect its quality.  Poac supports linting your project by the `lint` command:

```console
you:~/hello_world$ poac lint
   Linting hello_world
src/main.cc:0:  No copyright message found.  You should have a line: "Copyright [year] <Copyright Owner>"  [legal/copyright] [5]
Done processing src/main.cc
Total errors found: 1
Error: `cpplint` exited with status 1
```

> [!TIP]
> If you do not have `cpplint`, install it with the following command:
>
> ```bash
> pip install cpplint
> ```

The `lint` command works without configurations, and Poac would automatically opt out of unwanted lints by adjusting to each project.
To customize the lint settings, try adding the `[lint.cpplint]` key in your `poac.toml` like [this](https://github.com/poac-dev/poac/blob/cc30b706fb49860903384df56d650a0955aca16c/poac.toml#L67-L83)
or creating a [`CPPLINT.cfg`](https://github.com/poac-dev/poac/blob/5e7e3792e8818d165149214e94f30958fb0fef66/CPPLINT.cfg) file to the repository root.

### Run formatter

Poac also supports formatting your source code with `clang-format`.  Ensure having installed `clang-format` before running this command.

```console
you:~/hello_world$ poac fmt
  Formatting hello_world
```

> [!NOTE]
> This command automatically detects what files we need to format to avoid getting bothered by commands like:
>
> ```console
> $ # We need to avoid the `build` dir and such dirs ...
> $ clang-format ./src/*.cpp -i
> $ clang-format ./include/**/*.hpp -i
> $ clang-format ./tests/**/*.cpp -i
> $ ...
> ```

To customize the format settings, try creating a [`.clang-format`](/.clang-format) file to the repository root.

## Why Poac?

C++ is often considered a complex language and unconsciously avoided by many.  The absence of a definitive package manager and the unfamiliar syntax of build systems like [CMake](https://cmake.org) make it seem difficult to set up a C++ environment, leaving people hesitant.

To simplify the process and allow users to develop applications and libraries without worrying about [CMake](https://cmake.org), I created a package manager and build system with an intuitive interface, similar to [Cargo](https://github.com/rust-lang/cargo). This allows developers to focus on learning C++ without any hindrances. Additionally, I aim to integrate with other build systems and package managers, providing a seamless transition between development environments.

### Naming Background

Poac is originated from `cpp` but designed to prioritize ease of typing and reduce strain on one hand as it will be frequently used as a command. Its name is ergonomically optimized to prevent the development of tenosynovitis.

<img width="1230" alt="ergo" src="https://user-images.githubusercontent.com/26405363/169741684-ecb8aaad-2599-45ae-a680-a26f11652132.png">

Despite C++ often being overlooked for product development, I believe that Poac can help to promote it as a fun language. Amemiya and Mizutani argue that the sound of `/p/` is associated with a bright and soft impression among Japanese consonants (157)[^1]. In the same way, I believe that Poac's name can convey a similarly positive impression.

## Contributing

### Source Code Documentation

A source code documentation for Poac is generated by Doxygen every commit to the `main` branch:

https://dev.poac.dev

### Before submitting your PR

Please make sure to follow these steps:
<!-- > [!IMPORTANT]
> I expect you to use the latest `clang-tidy` and `clang-format`. -->

<!-- #### Run linter (`cpplint`)

```bash
poac lint
``` -->

#### Run formatter (`clang-format`)

```bash
poac fmt
```

---

This project exists thanks to all the people who contribute.

<a href="https://github.com/poac-dev/poac/graphs/contributors">
  <img src="https://contributors-img.web.app/image?repo=poac-dev/poac" />
</a>

## License

Poac is licensed under the terms of the Apache License version 2.0.

Please see [LICENSE](LICENSE) for details.

### Third-party software

* toml11 - <https://github.com/ToruNiina/toml11/blob/master/LICENSE>


[^1]: Amemiya, T., & Mizutani, S. (2006). On the Basic Affective Dimensions of Japanese Onomatopoeia and the Basic Level of Japanese Phonesthemes. 関西大学社会学部紀要, 37(2), 139–166. https://hdl.handle.net/10112/12311
