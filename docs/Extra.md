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

Target triple is specified in form `<target><vendor><platform>`. Examples of possible combinations:
* Generate 32bit x86 ELF:
`brb --triple=i686-elf [..]`
* Generate 64bit x86 ELF:
`brb --triple=x86_64-unknown-linux-gnu [..]`
* Generate 64bit ARM ELF:
`brb --triple=aarch64-unknown-linux-gnu [..]`
* Generate 64bit x86 PE/COFF
`brb --triple=x86_64-pc-windows-msvc [..]`