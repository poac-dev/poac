# poac
[![CircleCI](https://circleci.com/gh/poacpm/poac.svg?style=svg)](https://circleci.com/gh/poacpm/poac)
![C++ Standard](https://img.shields.io/badge/C%2B%2B-17-blue.svg?style=flat-square)

**It is prohibited to use -(hyphen) as a package name and _ (under bar) is recommended.**

**Also the use of capital letters is prohibited. In other words, please use snake case.**

**SAT base**

## Installation
### Using [Homebrew](https://github.com/Homebrew/brew) (macOS)
```bash
$ brew install poac
$ poac -v
```
### Manually (Other OS)
```bash
$ git clone $THIS_REPO
$ cd poac
$ mkdir build
$ cd $_
$ cmake ..
$ make install
```

## Requirements
* `boost`: version `1.67.0` or higher
* `cmake`: version ``
* `curl(libcurl)`:


## Documentation
Please look at [github.io](https://poacpm.github.io/poac/) or [This repo](docs/).

## Contribution
[View Contribution Guidelines](.github/CONTRIBUTUING.md)
```bash
$ mkdir build
$ cd $_
$ cmake ..
$ make
$ CTEST_OUTPUT_ON_FAILURE=TRUE make test
```
