## Hello World

To start a new project with poac, use `poac new`:
```bash
$ poac new hello_world

Your "hello_world" project was created successfully.


Go into your project by running:
    $ cd hello_world

Start your project with:
    $ poac install
    $ poac run
```

Check out to project directory.
```bash
$ cd hello_world
$ tree .
.
├── poac.yml
└── main.cpp

0 directory, 2 files
```

`poac.yml` is the settings file.


Poac generated a “hello_world” binary for us. Let’s compile it:
```bash
$ poac build
Compiled: Output to `_build/bin/hello_world`

$ ./_build/bin/hello_world
Hello, world!
```

We can also use `poac run` to compile and then run it, all in one step:
```bash
$ poac run
Compiled: Output to `_build/bin/hello_world`
Running: `_build/bin/hello_world`
Hello, world!
```
