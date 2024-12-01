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

### Installing from Source

Although building from source is not recommended, if you really want to do so, see [INSTALL.md](INSTALL.md).

### Runtime Requirements

* C++ compiler
* GNU Make
* pkg-config
* mkdir
* printf
* cpplint (for `poac lint`)
* clang-format (for `poac fmt`)
* clang-tidy (for `poac tidy`)

## Community

See [GitHub Discussions](https://github.com/orgs/poac-dev/discussions).

## Contributing

See [CONTRIBUTING.md](CONTRIBUTING.md).

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
