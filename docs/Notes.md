## Bits Runner Builder, version 1.0.0 (under development)

Initial realease for the BRC language. Prima sort 👌

Available for:
- macOS (x86_64 and arm64)
- Linux (x86_64)
- Windows (x86_64)

Changelist:
- [1.0.0-dev-103](https://github.com/rafalgrodzinski/bits-runner-builder/pull/197)
  - Implicit sub-imports import delcarations for blobs and protos
  - Global names for types, so `blob<@main::something>` and `blob<something>` will refer to the same type in `main` module
  - Namespaces can be used for blob types
  - Verify that index access is used on `data`
  - Moved libraries into `lib/brc/`
  - Updated docs
