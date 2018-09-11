**:warning: Not yet released**

# poac [![CircleCI](https://circleci.com/gh/poacpm/poac.svg?style=svg)](https://circleci.com/gh/poacpm/poac)

Poac is the package manager for C/C++ user.

Poac can downloads project's dependencies and compiles project.

Please see [poac.pm](https://poac.pm) for [installation instructions](https://poacpm.github.io/poac/en/getting-started/installation.html) and [other documentation](https://poacpm.github.io/poac/).


## Supported Operating Systems

Poac currently supports macOS only.


## Installation
### Easy install
```bash
curl https://sh.poac.pm | sh
```
*When your OS is macOS, use [Homebrew](https://github.com/Homebrew/brew)*

### Manual (Build)
Poac requires the following tools and packages to build:
* `boost`: `1.48.0` or higher
* `cmake`: `3.0` or higher
* `curl(libcurl)`:
* openssl
* `yaml-cpp`: `0.6.0` or higher

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
$ poac build --release
```


## Requirements (runtime)
* compiler (gcc | clang | MSVC | ICC)
* `tar`
* `git`
* `cmake`
* `make`


## Contribution
See [CONTRIBUTUING.md](.github/CONTRIBUTUING.md).
