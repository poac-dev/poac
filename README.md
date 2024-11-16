<div align="center">
  <picture>
    <source media="(prefers-color-scheme: dark)" srcset="https://user-images.githubusercontent.com/26405363/170157214-51b03bbd-cd29-432b-99ca-8f65c01f973e.svg">
    <source media="(prefers-color-scheme: light)" srcset="https://user-images.githubusercontent.com/26405363/170157184-e338188c-e4fa-4967-ac74-aba03aefa0bc.svg">
    <img alt="Poac Logo" width="250" src="https://user-images.githubusercontent.com/26405363/170157184-e338188c-e4fa-4967-ac74-aba03aefa0bc.svg">
  </picture>
</div>

> [!WARNING]
> The information here may vary depending on the version you are using.  Please refer to the corresponding `README.md` by visiting the Git tag corresponding to your version, e.g., https://github.com/poac-dev/poac/blob/0.9.3/README.md.  Running `poac version` will provide you with the current version.

## Description

> [!CAUTION]
> Poac is still under development and may contain a bunch of bugs.

![demo](https://vhs.charm.sh/vhs-5dQzA2VI3B0e4jTAC6RrME.gif)

Poac (pronounced as `/pəʊək/`) is a package manager and build system for C++ users, inspired by Cargo for Rust.  Poac is designed as a structure-oriented build system, which means that as long as you follow Poac's designated project structure, you almost do not need configurations, much less a language to build, unlike CMake.  If you do not like writing a bunch of configurations to build your project, Poac might be best suited.  Currently, the supported project structure can be known by looking at this repository since Poac can build itself.

Please visit [poac.dev](https://poac.dev) and [Poac Docs](https://poac.dev/docs) for more details.

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

#### AUR (Arch Linux)

Poac is available in the AUR thanks to the [poac](https://aur.archlinux.org/packages/poac) package.

It can be installed with the AUR helper of your choice.

```sh
paru -S poac
```

### Build from source

If your environment is not included in the released packages, you have to build Poac from the source. You will require the following compilers, commands, and libraries:

#### Compilers (that support C++20)

* GCC: `12` or later
* Clang: `16` or later
* Apple Clang: provided by `macOS Ventura (13)` or later

(generally, the latest 3 versions are supported to build Poac.)

#### Commands

* GNU Make
* Git
* pkg-config
* find
* grep
* mkdir
* rm

#### Libraries

* fmt: `>=8.1.1 && <12`
    * `libfmt-dev` on APT (Debian/Ubuntu)
    * `fmt-devel` on DNF (Fedora)
    * `fmt` on Homebrew
* libgit2: `>=1.1.0 && <1.9`
    * `libgit2-dev` on APT (Debian/Ubuntu)
    * `libgit2-devel` on DNF (Fedora)
    * `libgit2` on Homebrew
* libcurl: `>=7.79.1 && <9`
    * `libcurl4-openssl-dev` on APT (Debian/Ubuntu)
    * `libcurl-devel` on DNF (Fedora)
    * `curl` on Homebrew
* nlohmann_json: `>=3.10.5 && <4`
    * `nlohmann-json3-dev` on APT (Debian/Ubuntu)
    * `json-devel` on DNF (Fedora)
    * `nlohmann-json` on Homebrew
* oneTBB: `>=2021.5.0 && <2023`
    * `libtbb-dev` on APT (Debian/Ubuntu)
    * `tbb-devel` on DNF (Fedora)
    * `tbb` on Homebrew

Installation scripts for libraries:
* APT (Debian/Ubuntu):
  ```sh
  sudo apt-get update
  sudo apt-get install -y libfmt-dev libgit2-dev libcurl4-openssl-devnlohmann-json3-dev libtbb-dev
  ```
* DNF (Fedora):
  ```sh
  sudo dnf install -y fmt-devel libgit2-devel libcurl-devel json-devel tbb-devel
  ```
* Homebrew:
  ```sh
  brew install fmt libgit2 curl nlohmann-json tbb
  ```

When running Make, the following libraries will be installed automatically.

* [toml11](https://github.com/ToruNiina/toml11): [`v4.2.0`](https://github.com/ToruNiina/toml11/releases/tag/v4.2.0)

Once you have all the necessary requirements in place, you can build Poac by the following commands:

```bash
git clone https://github.com/poac-dev/poac.git
cd poac
make RELEASE=1 install
```

### Runtime Requirements

* C++ compiler
* GNU Make
* pkg-config
* mkdir
* printf
* cpplint (for `poac lint`)
* clang-format (for `poac fmt`)
* clang-tidy (for `poac tidy`)

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

> [!TIP]
> To use a different compiler, you can export a `CXX` environmental variable:
>
> ```shell
> export CXX=g++-13
> ```

### Install dependencies

Like Cargo does, Poac installs dependencies at build time.  Poac currently supports Git and system dependencies.  You can use the `poac add` command to add dependencies to your project.

The `poac add` command accepts the following arguments:

`poac add <package names ....> --<options>`

Options:
- `--sys`: Marks the packages as system dependency (requires the `--version` argument)
- `--version`: Specify dependency version. Only used with system dependencies
- `--rev`: Specify revision for git dependencies
- `--tag`: Specify tag for git dependencies
- `--branch`: Specify branch for git dependencies

Example
```bash
poac add libgit2 --sys --version "1.1.0"
poac add "ToruNiina/toml11" --rev "846abd9a49082fe51440aa07005c360f13a67bbf"
```

If `tag`, `branch`, or `rev` is unspecified for git dependencies, Poac will use the latest revision of the default branch. System dependency names must be acceptable by `pkg-config`. The version requirement syntax is specified in [src/VersionReq.hpp](https://github.com/poac-dev/poac/blob/main/src/VersionReq.hpp).

After adding dependencies, executing the `build` command will install the package and its dependencies.

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

### Unit tests

You can write unit tests in any source files within the `src` directory.  Create a new file like:

`src/Lib.cc`

```cpp
int add(int a, int b) {
  return a + b;
}

#ifdef POAC_TEST

#  include <cassert>

int main() {
  assert(add(1, 2) == 3);  // ok
  assert(add(1, 2) == 4);  // fail
}

#endif
```

Now, with the `test` command, you can run tests defined within `POAC_TEST`:

```console
you:~/hello_world$ poac test
 Compiling src/Lib.cc
   Linking tests/test_Lib
   Testing Lib
Assertion failed: (add(1, 2) == 4), function main, file Lib.cc, line 13.
make: *** [test] Abort trap: 6
```

Unit tests with the `POAC_TEST` macro are useful when testing private functions.  Integration testing with the `tests` directory has not yet been implemented.

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
or creating a [`CPPLINT.cfg`](https://github.com/poac-dev/poac/blob/5e7e3792e8818d165149214e94f30958fb0fef66/CPPLINT.cfg) file in the repository root.

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

### Run `clang-tidy`

Poac also supports running `clang-tidy` on your source code.  Ensure having installed `clang-tidy` before running this command.

```console
you:~/hello_world$ poac tidy
  Running clang-tidy
```

You can customize the tidy settings by creating a [`.clang-tidy`](/.clang-tidy) file to the repository root.

## Why Poac?

C++ is often considered a complex language and unconsciously avoided by many.  The absence of a definitive package manager and the unfamiliar syntax of build systems like [CMake](https://cmake.org) make it seem difficult to set up a C++ environment, leaving people hesitant.

To simplify the process and allow users to develop applications and libraries without worrying about [CMake](https://cmake.org), I created a package manager and build system with an intuitive interface, similar to [Cargo](https://github.com/rust-lang/cargo). This allows developers to focus on learning C++ without any hindrances. Additionally, I aim to integrate with other build systems and package managers, providing a seamless transition between development environments.

### Naming Background

Poac is originated from `cpp` but designed to prioritize ease of typing and reduce strain on one hand as it will be frequently used as a command. Its name is ergonomically optimized to prevent the development of tenosynovitis.

<img width="1230" alt="ergo" src="https://user-images.githubusercontent.com/26405363/169741684-ecb8aaad-2599-45ae-a680-a26f11652132.png">

Despite C++ often being overlooked for product development, I believe that Poac can help to promote it as a fun language. Amemiya and Mizutani argue that the sound of `/p/` is associated with a bright and soft impression among Japanese consonants (157)[^1]. In the same way, I believe that Poac's name can convey a similarly positive impression.

## Community

We use [GitHub Discussions](https://github.com/orgs/poac-dev/discussions).

## Acknowledgements

This project exists thanks to all the people who contribute.

<a href="https://github.com/poac-dev/poac/graphs/contributors">
  <img src="https://contributors-img.web.app/image?repo=poac-dev/poac" />
</a>

<a href="https://star-history.com/#poac-dev/poac&Date">
 <picture>
   <source media="(prefers-color-scheme: dark)" srcset="https://api.star-history.com/svg?repos=poac-dev/poac&type=Date&theme=dark" />
   <source media="(prefers-color-scheme: light)" srcset="https://api.star-history.com/svg?repos=poac-dev/poac&type=Date" />
   <img alt="Star History Chart" src="https://api.star-history.com/svg?repos=poac-dev/poac&type=Date" />
 </picture>
</a>

## License

Poac is licensed under the terms of the Apache License version 2.0.

Please see [LICENSE](LICENSE) for details.

### Third-party software

* toml11 - <https://github.com/ToruNiina/toml11/blob/master/LICENSE>


[^1]: Amemiya, T., & Mizutani, S. (2006). On the Basic Affective Dimensions of Japanese Onomatopoeia and the Basic Level of Japanese Phonesthemes. 関西大学社会学部紀要, 37(2), 139–166. https://hdl.handle.net/10112/12311
