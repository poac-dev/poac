# poac
[![CircleCI](https://circleci.com/gh/poacpm/poac.svg?style=svg)](https://circleci.com/gh/poacpm/poac)

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

## Contribute
```bash
$ mkdir build
$ cd $_
$ cmake ..
$ make
$ CTEST_OUTPUT_ON_FAILURE=TRUE make test
```
