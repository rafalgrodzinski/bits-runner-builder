# Extra Information

- LLVM Language Reference Manual
[https://llvm.org/docs/LangRef.html](https://llvm.org/docs/LangRef.html)
- Processing command line arguments with LLVM
[https://llvm.org/docs/CommandLine.html](https://llvm.org/docs/CommandLine.html)
- What the Hell Is a Target Triple?
[https://mcyoung.xyz/2025/04/14/target-triples/](https://mcyoung.xyz/2025/04/14/target-triples/)

## Code Generation Target
How to get information about possible target triples and architectures:
* List available target architectures:
`clang -mcpu=help`
* Show default target triple:
`clang -dumpmachine`
* List available targets:
`clang -print-targets`
* List architectures(CPUs) and features for a target:
`llc -march=<target> -mattr=help`

## Target Triple
`--triple=<string>`
Target triple is specified in form `<target><vendor><platform>`. Examples of possible combinations:
* Generate 32bit x86 ELF:
`brb --triple=i686-elf [..]`
* Generate 64bit x86 ELF:
`brb --triple=x86_64-unknown-linux-gnu [..]`
* Generate 64bit ARM ELF:
`brb --triple=aarch64-unknown-linux-gnu [..]`
* Generate 64bit x86 PE/COFF
`brb --triple=x86_64-pc-windows-msvc [..]`

## Relocation Model
`--reloc=[static|pic]`
Addresses can be resolved either through linker (static) or loader (pic). PIC (position independent code) allows, for example, that a shared library gets loaded into any memory address, but requires additional support from loader. Static relocation means that when the code is linked, the final address is determined and cannot be changed. It's less flexible, but has lower overhead, doesn't require any additional suppor and can be used for flat binnaries (when running on bare metal).

## Code Model
`--code=[tiny|small|kernel|medium|large]`
This option affects how addresses are calculated. It forms a part of ABI and is platform specific. [System V ABI](https://raw.githubusercontent.com/wiki/hjl-tools/x86-psABI/x86-64-psABI-1.0.pdf) and [this slack overflow question](https://stackoverflow.com/questions/40493448/what-does-the-codemodel-in-clang-llvm-refer-to) should have some more information on it.

## Optimization Level
`--opt=[g|O0|O1|O2|O3]`
Details on what each option do are burries somewhere inside of LLVM's source code, but in short `O2` is the sane default, whereas `O3` might be a bit more performant but also unstable. In general everything should work correctly at `O2`.

## Additional Options
`--function-sections` Places each function in a separate section, which can be useful if we want to manually arrange functions in a linker script. For example to place startup code at the beginning.

`--no-zero-initialized-in-bss` By default zero-initialized global data is not included in the binary but rather just marked as existing in the `.bss`. This requires a loader support to work properly. We can disable this so even zero-initialized global variables will be directly placed in the `.data` section, which is useful when generating flat binaries.