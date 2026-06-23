# Bits Runner Builder
Bits Runer Code (BRC) is a low-level system language for [Bits Runner](https://github.com/rafalgrodzinski/bits-runner) Operating System project. It aims to be a modernised C with s clear and explicit syntax. Bits Runner Builder (BRB) is its implementation. It's available for macOS, Linux, and Windows.

### In this readme
- [A taste of BRC](README.md#a-taste-of-brc)
- [But why?](README.md#but-why)
- [How to install](README.md#how-to-install)
- [How to build](README.md#how-to-build)
- [Samples](README.md#samples)
- [Visual Studio Code support](README.md#visual-studio-code-support)
- [Further resources](README.md#further-resources)


## A taste of BRC
```
// main.brc

// Generate an array of randome numbers and print them
@import B

@extern rand fun -> u64
@extern srand fun: seed u64
@extern time fun: second u64 -> u64

@export main fun -> u32
    // Initialize randome number generator
    srand(time(0))

    // Initialize an array with 10 randomly generated numbers
    numbers blob<@B::Array, u32>
    rep i u32, i < 10, i <- i + 1: numbers.push((rand() % 1_000).u32)

    // Print out the numbers
    rep i u32, i < numbers.count, i <- i + 1
        @B::StringForU32(numbers.at(i).u32).print()
        @B::String(" ").print()
    ;

    @B::String("\n").println()

    ret 0
;
```

If `brb` has been installed with Homebrew, you can build and run it using:

```
brb main.brc "`brew --prefix`/lib/brc/BSys.brc" "`brew --prefix`/lib/brc/B/String.brc" "`brew --prefix`/lib/brc/B/Array.brc"
cc -o taste main.o BSys.o B.o
./taste
```


## But why?
The idea was to build the whole computing environment from scratch which can be its own thing. Many project try to be a sort of recreation of C/Unix, but this is not the point here. This project doesn't aim at compatibility with existing sfotware so it may hapilly break things in order to make them simpler, more modern, or simply different.

It aims to be an opinionated, low-level system language, a modernised C with revised syntax and a number of quality of life improvement. It is explicit and clear, so what you write is what you get. No runtime, implicit startup code, or templates. Altough it has a simple class-like functionality using `blob`, class hierarchies, templates, or other unnecessary fluff is not supported. It alows for interface types with `proto` and generic/template/union like functionality with `boxed<T>` type.

The syntax is different, instead of curcly braces for blocks, thers is only a semicolon at the end of one. Which means no semicolons after statements. There are no header, instead there are modules which can be made out of multiple files.

BRC allows for low-level system programming, so one of the main features is a seamless support for inline assembly, pointers, and explicit data handling. For this reason types have explicit sizes, there is no runtime and the memory is manually managed.

It has been been built with LLVM so it should be fairly performant. Keep in mind that it is still work in progress so not everything is finished and there is still probably plenty of bugs and gremlins hiding around 🐝 If you find any, let me know, additional pair of eyes is always helpful.

It's mostly a learning opportunity and a bit of fun, but maybe you can find some bits of interesting knowledge for your own project.

### Some highligths
- Modules without headers
- Pointers use instance properties instead of operators
- Casting also uses member syntax
- No curly braces for scope or semicollons
- Functions inside of `blob` types for simple class-like funtionality
- Interfaces with `proto` types
- `if-else` statements are expressions
- Explicitly sized types (integers, floats, etc)
- `for`, `while`, `do-while` loops integrated into single `rep` keyword
- Directly supports decimal, hex, binary numbers with `_` separator between digits
- Bit test `&?` operator
- Shows tokens, AST, and build statistics for each phase with `--verb=v2` or `--verb=v3`


## How to Install
To try out BRC download an appropriate build of brb from [releases](https://github.com/rafalgrodzinski/bits-runner-builder/releases) and make sure that you have LLVM 20 installed on your system.

### macOS
Tested on macOS 15 (Sequoia)
- Make sure you have [Homebrew](https://brew.sh/) installed
- Install using homebrew through my personal tap `brew install rafalgrodzinski/tap/brb`
- Or install manually by first nstalling LLVM: `brew install llvm@20`
- Then get the latest macOS build from [releases](https://github.com/rafalgrodzinski/bits-runner-builder/releases): `brb-<version>-<build>-macos-<arch>.zip`

### Linux
Tested on Ubuntu 24
- LLVM: `sudo apt install llvm-20`
- Get latest Linux build from [releases](https://github.com/rafalgrodzinski/bits-runner-builder/releases): `brb-<version>-<build>-linux-x86_64.zip`

### Windows
Tested on Windows 11, but should work fine for other 64 bit windows too. Windows version has statically linked LLVM, so no additional libraries are required.
- Get latest Windows build from [releases](https://github.com/rafalgrodzinski/bits-runner-builder/releases): `brb-<version>-<build>-win-x86_64.zip`


## How to Build?
Required dependencies
- clang 17.0.0 and up
- Visual Studio 2026
- LLVM 20.1.0
- CMake 4.0.0 and up

### macOS
- Just as for installation, first get [Homebrew](https://brew.sh/) installed
- `brew install llvm@20` Install LLVM
- `brew install cmake` Install cmake
- `cmake -B build` Configure the project
- `cmake --build build --config Relese` (or `--config RelWithDebInfo`) Build the poject
- Binary should be under `build/brb`
- `cmake --build build --target install` Install in system
- `bin/brb` will be the executables, `lib/brc/` will contain the standard libraries

### Linux
- CMake: Version 4 wasn't available with apt-get, so it had to be installed manually from [cmake.org](https://cmake.org/download/)
- Install LLVM: `sudo apt install llvm-20`
- build process is the same as for macOS

### Windows
Windows build is a little bit more involved because you need to setup the environment and build LLVM from sources. Available LLVM binaries don't contain the necessary libraries.
- Instal `Visual Studio 2026` (any version) or just the `Build Tools for Visual Studio 2026` from (here)[https://visualstudio.microsoft.com/ja/downloads/]. Make sure that you install C++ and CMake components.
- Setup `PATH` environment variable so it includes the `cmake.exe` location at
- Download and unzip (LLVM 20.1.8 sources)[https://github.com/llvm/llvm-project/archive/refs/tags/llvmorg-20.1.8.zip] from release page and unzip them. Or run: `git clone git@github.com:llvm/llvm-project.git --branch llvmorg-20.1.8 --depth 1`
- Run Visual Studio console `x64 Native Tools Command Prompt for VS`, go to the source folder and first configure the project: `cmake -DCMAKE_BUILD_TYPE=Release -DCMAKE_INSTALL_PREFIX=C:\llvm-20.1.8 -Thost=x64 -B build -S llvm`
- Once that succeeded, build the project by running: `cmake --build build --config Release`. This can take anything from 10 minutes to 2-3 hours, depending on your hardware.
- And finally install by running: `cmake --install build --prefix C:\llvm-20.1.8`
- Now that LLVM is ready, go to brb source and configure: `cmake -B build -DCMAKE_PREFIX_PATH=C:\llvm-20.1.8`
- And finally build it: `cmake --build build --config Release` (or `--config Debug`)
- You'll then be able to find the executable under `build/Release/brb`.


## Samples
Inside of `samples/` there is a bunch of different sample code using different features of the code. Inside each of them there is a `build.sh` and `build.bat`, which will generate an executable. You can also Run `samples/run_all.sh` to build and run each of the samples, which is useful for testing to check if everything works as expected.

`callback`:
Uses function pointers to implement basic callback functionality.

`casts`:
Demonstrates how to use casting for for simple and data types.

`date`:
Sample use of the `@B::Date`.

`dyn_array`:
Sample use of the `@B::Array`.

`external_linkage`:
Builds two separate object files and then links them together.

`fib`:
Fibonaci numbers, demonstrates recursion.

`fizz_buzz`:
Fizz buzz is useful for easily figuring out if someone know anythign about programming, but here is uses multi-level if-else expressions with more complex conditions.

`hello`:
No programming language is complete without a hello world.

`interfaces`:
Sample of how to use `proto`.

`linked_list`:
Uses pointers and memory allocation for implementing a single linked list.

`merge_sort`:
Implementation of merge sort to show usage of data types.

`multi_module`:
Shows how code can be split into separat files and modules.

`primes`:
Based on ["Software Drag Racing!" by Dave Plummer](https://github.com/PlummersSoftwareLLC/Primes/tree/drag-race). Runs as many iterrations as possible of calculating primes up to 1,000,000 in 5 seconds and reports the number or iterrations. I got a bit under 1200 on my Intel Core i7 14700.

`strings`:
This uses the basic library `@B` to manipulate strings.

`interfaces`:
Shows how `proto` can be used for indirect useage of `blob`.

### Tests
There is a bunch of small tests that are used to check correctness of the generated code. They can be run individually or together by running `tests/run_all.sh`. It can also be useful to see additional usage examples.

### Bits Runner
For a real use of the language checkout sources for the [Bits Runner](https://github.com/rafalgrodzinski/bits-runner) project.


## Visual Studio Code support
There are "Build (Debug)" and "Clean" tasks specified for VSCode. There is also a launch configuartion, which you can launch by pressing F5 which will then build and start debugging using command `brb --verb=v3 samples/test.brc`. You'll need to have "LLDB DAP" extension installed in VSCode and `lldb-dap` on your system. There is also [a BRC langauge extension](https://github.com/rafalgrodzinski/brc-vscode) available.


## Further resources
- [Language reference](docs/Reference.md)
- [Casting rules](docs/Casts.md)
- [Extra information](docs/Extra.md)
- [BRC language extension for Visual Studio Code](https://github.com/rafalgrodzinski/brc-vscode)
- [Bits Runner OS](https://github.com/rafalgrodzinski/bits-runner)