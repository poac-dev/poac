# poac
[![CircleCI](https://circleci.com/gh/poacpm/poac.svg?style=svg)](https://circleci.com/gh/poacpm/poac)
![C++ Standard](https://img.shields.io/badge/C%2B%2B-17-blue.svg?style=flat-square)

**It is prohibited to use -(hyphen) as a package name and _ (under bar) is recommended.**

**Also the use of capital letters is prohibited. In other words, please use snake case.**

**SAT base**

## Install
### With homebrew
```bash
$ brew install poac
$ poac -v
```
### Manual
depends on
* boost
* cmake
* curl(libcurl)
```bash
$ git clone $THIS_REPO
$ cd poac
$ mkdir build
$ cd $_
$ cmake ..
$ make install
```

## Documents
Please look at [github.io](https://poacpm.github.io/poac/) or [This repo](docs/SUMMARY.md).

## Contribute
[View Contribution Guidelines](.github/CONTRIBUTUING.md)
```bash
$ mkdir build
$ cd $_
$ cmake ..
$ make
$ CTEST_OUTPUT_ON_FAILURE=TRUE make test
```
