# Caresto

## Windows

### Getting a C compiler

Download and install LLVM: https://releases.llvm.org/ (using 19.1.0)

Download and install Visual Studio Installer: https://visualstudio.microsoft.com

In Visual Studio Installer add "Game Development with C++"


### Building

You need:
- A way to execute shell scripts (you can use `sh.exe` from Git);
- `wget` to download dependencies;
- `make` to build.

Resolve project dependencies.

```sh
./dependencies.sh
``` 

If that doesn't work, use `sh.exe` from Git.

Make sure `C:\Program Files\Git\bin` is in your `PATH`.

```sh
sh ./dependencies.sh
```

Then you can build.

```sh
make build
```

Or build and run.

```sh
make run
```


### Dev

Generate the config files for clangd (LSP).

```sh
make dev
```

