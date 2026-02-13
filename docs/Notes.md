## Bits Runner Builder, version 1.0.0 (under development)

Initial realease for the BRC language. Prima sort 👌

Available for:
- macOS (x86_64 and arm64)
- Linux (x86_64)
- Windows (x86_64)

Changelist:
- [https://github.com/rafalgrodzinski/bits-runner-builder/pull/160](https://github.com/rafalgrodzinski/bits-runner-builder/pull/160)
  - Sped up analysis
  - Zero uninitialized local variables
  - Restrict usage of `@export`, and intitializer on blob members
- [https://github.com/rafalgrodzinski/bits-runner-builder/pull/151](https://github.com/rafalgrodzinski/bits-runner-builder/pull/151)
  - Builds and runs on windows
  - Automatic relase with github actions
- [https://github.com/rafalgrodzinski/bits-runner-builder/pull/155](https://github.com/rafalgrodzinski/bits-runner-builder/pull/155)
  - Functions and can now be inccluded in blobs
- [https://github.com/rafalgrodzinski/bits-runner-builder/pull/159](https://github.com/rafalgrodzinski/bits-runner-builder/pull/159)
  - Added bit test operator `&?`
  - Raw functions are now exportable
- [https://github.com/rafalgrodzinski/bits-runner-builder/pull/153](https://github.com/rafalgrodzinski/bits-runner-builder/pull/153)
  - Allow for such syntax `numAdr.a.ptr<u32>.val`
  - Updated base String functions
  - Ambiguous `>>` and `<<` is now deducted correctly
  - Added `u16` and `s16` types
- [https://github.com/rafalgrodzinski/bits-runner-builder/pull/152](https://github.com/rafalgrodzinski/bits-runner-builder/pull/152)
  - Fixed pointers when O2 optimization is enabled
  - Specialized `a` type for handling addresses
- [https://github.com/rafalgrodzinski/bits-runner-builder/pull/148](https://github.com/rafalgrodzinski/bits-runner-builder/pull/148)
  - Use new pass manager for optimizations
- [https://github.com/rafalgrodzinski/bits-runner-builder/pull/147](https://github.com/rafalgrodzinski/bits-runner-builder/pull/147)
  - Optimized builds
  - Using wrapped values all over the place
  - Bunch of various bug fixes
- [https://github.com/rafalgrodzinski/bits-runner-builder/pull/143](https://github.com/rafalgrodzinski/bits-runner-builder/pull/143)
  - Single line if-else can be embeded in an expression
  - Added @B.Date which can convert date components to a timestamp
  - @BSys which acts as a layer between OS and the B library
  - @B.String can now print hex, signed, and floating point numbers
  - Casting between types has been fixed
  - Multiple analyzer crashes fixed
- [https://github.com/rafalgrodzinski/bits-runner-builder/pull/142](https://github.com/rafalgrodzinski/bits-runner-builder/pull/142)
  - Dispalys file name in error messages. Useful when a single module is split into multiple files.
  - Updated parsing of if-else statements