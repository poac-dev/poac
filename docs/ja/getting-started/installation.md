## Installation

### 簡単にインストールする
```bash
curl https://sh.poac.pm | bash
```
*お使いのPCのOSがmacOSの時、内部では [Homebrew](https://github.com/Homebrew/brew) を使用します*

### 手動でインストールする (ビルド)
poac はビルドするために以下のツールとパッケージが必要です:
* `boost`: version `1.48.0` もしくはそれ以上
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

既に poac がインストール済みであれば、poac を使用してビルドすることが可能です:
```bash
$ poac build
```
