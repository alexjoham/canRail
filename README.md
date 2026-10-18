# canRail

`canRail` is a re-implementation of a model railway CAN controller for the
Arduino Uno and MCP2515 CAN hardware.

## Protocol reference

The CAN protocol implementation is based on Märklin's **CS2 CAN protocol
specification, version 1.0**:

<https://www.maerklin.de/fileadmin/media/produkte/CS2_can-protokoll_1-0.pdf>

The protocol types and identifiers in this repository are derived from that
document. This project is an independent re-implementation and is not
affiliated with Märklin.

When adding or changing protocol behavior:

1. Check the corresponding command and payload definition in the
   specification.
2. Add a test using a known protocol value or byte sequence.
3. Keep protocol encoding and decoding separate from hardware transport code.

## Project structure

```text
include/
  protocol/       Typed CAN protocol data and codecs
  transport/      Hardware-independent transport interfaces
  application/    Application-level CAN handling
src/
  main.cpp        Arduino application entry point
test/
  test_can_id/    Native protocol tests
```

Protocol messages should remain value types. Hardware-specific code belongs in
the transport or application layers and should not construct CAN identifiers or
payload bytes directly.

## Build and test

Build all PlatformIO environments:

```bash
pio run
```

Build only the Arduino Uno firmware:

```bash
pio run -e uno
```

Run the native protocol tests:

```bash
pio test -e native
```

Format C++ sources:

```bash
find include src test -type f \( -name '*.h' -o -name '*.hpp' -o -name '*.cpp' \) \
  -print0 | xargs -0 clang-format -i
```

Check formatting without modifying files:

```bash
clang-format --dry-run --Werror \
  $(find include src test -type f \( -name '*.h' -o -name '*.hpp' -o -name '*.cpp' \))
```
