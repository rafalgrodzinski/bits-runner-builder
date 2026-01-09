## Bits Runner Builder, version 1.0.0 (under development)

Initial realease for the BRC language. Prima sort 👌

Available for:
- macOS (Intel and Apple Silicon)
- Linux (x86_64)

Changelist:
- Invalid detection of shift #140
  - Allow for such syntax `numAdr.a.ptr<u32>.val`
  - Updated base String functions
- Better handling of addresses #150
  - Fixed pointers when O2 optimization is enabled
  - Specialized `a` type for handling addresses
-  145 slow build of big arrays #147 
  - Optimized builds
  - Using wrapped values all over the place
  - Bunch of various bug fixes
- Data casting not correct #94
  - Single line if-else can be embeded in an expression
  - Added @B.Date which can convert date components to a timestamp
  - @BSys which acts as a layer between OS and the B library
  - @B.String can now print hex, signed, and floating point numbers
  - Casting between types has been fixed
  - Multiple analyzer crashes fixed
- Pass file name together with line and column #124
  - Dispalys file name in error messages. Useful when a single module is split into multiple files.
  - Updated parsing of if-else statements