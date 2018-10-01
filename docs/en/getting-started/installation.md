## Installation

### Easy install
```bash
curl https://sh.poac.pm | bash
```
*When your OS is macOS, use [Homebrew](https://github.com/Homebrew/brew)*

### Manual install (Build)
Poac requires the following tools and packages to build:
* `boost`: version `1.48.0` or higher
* `cmake`: version ``
* `curl(libcurl)`:
* openssl
* `yaml-cpp`:

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
$ poac build
```
