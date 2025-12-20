## Bits Runner Builder, version 1.0.0 (under development)

Initial realease for the BRC language. Prima sort 👌

Available for:
- macOS (Intel and Apple Silicon)
- Linux (x86_64)

Changelist:
- Data casting not correct #94
  - Single line if-else can be embeded in an expression
  - Added @B.Date which can convert
  - @BSys which acts as a layer between OS and the B library
- Pass file name together with line and column #124
  - Dispalys file name in error messages. Useful when a single module is split into multiple files.
  - Updated parsing of if-else statements