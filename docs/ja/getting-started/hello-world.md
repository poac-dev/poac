# Getting Started
## Hello World

```bash
$ poac new hello_world

Your "hello_world" project was created successfully.


Go into your project by running:
    $ cd hello_world

Start your project with:
    $ poac install hello_world
    $ poac run main.cpp


```
poac use `poac.yml` as setting file.

```yaml:poac.yml
# This is a comment.
name: sample
version: 0.0.1
cpp_version: ""
description: "This is a Sample file."
authors:
  - "good author"
license: ISC
links:
  - GitHub: https://github.com
deps:
  - hello_world: 1.0.0
```



