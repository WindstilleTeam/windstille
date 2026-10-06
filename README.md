# miniswig

Binding generator for the [Squirrel](http://www.squirrel-lang.org/) scripting
language. It reads preprocessed C++ headers and emits Squirrel wrapper code
(`.cpp` / `.hpp`) so C++ types and functions can be called from scripts.

miniswig is used by projects such as SuperTux to expose game APIs to Squirrel.

## Requirements

- CMake ≥ 3.15
- A C++17 compiler
- flex and bison
- (optional, for tests) [Squirrel](https://github.com/albertodemichelis/squirrel) 3.2

With [Nix](https://nixos.org/):

```bash
nix build
nix develop   # shell with build tools
```

## Building

```bash
cmake -B build -DBUILD_TESTS=ON
cmake --build build
ctest --test-dir build --output-on-failure
```

Or via Nix (tests enabled when not cross-compiling to Windows):

```bash
nix build
```

## Usage

1. Preprocess a C++ header that declares the scripting API (mark the public
   surface with `SCRIPTING_API` or similar, and use the miniswig annotations
   below).
2. Run miniswig on the preprocessed output.
3. Compile the generated wrapper into your application and register it with
   the Squirrel VM.

```bash
# Preprocess
c++ -x c++ -E -CC my_api.hpp -o my_api.ii -DSCRIPTING_API

# Generate wrappers
miniswig \
  --input my_api.ii \
  --output-cpp my_api_wrap.cpp \
  --output-hpp my_api_wrap.hpp \
  --module my_api \
  --select-namespace my_api
```

### Command-line options

| Option | Description |
|--------|-------------|
| `--input FILE` | Preprocessed C++ translation unit |
| `--output-cpp FILE` | Generated C++ wrapper implementation |
| `--output-hpp FILE` | Generated C++ wrapper header |
| `--output-hpp-include PATH` | Include path written into the `.cpp` (defaults to the hpp path) |
| `--input-hpp PATH` | Include path for the original API header |
| `--module NAME` | Module / registration name |
| `--select-namespace NAME` | Only emit bindings for this C++ namespace |
| `--output-doc FILE` | Optional XML documentation dump |
| `--version` | Print version and exit |
| `--help` / `-h` | Usage |

### Annotations

In headers processed by miniswig (when `SCRIPTING_API` is defined the macros
expand to nothing for normal compilation):

- `__suspend` — mark a function that should suspend the Squirrel VM
  (cooperative multitasking).
- `__custom("signature")` — mark a hand-written `SQInteger(HSQUIRRELVM)`
  function; the string is a Squirrel parameter type check string.

See `tests/example.hpp` for a complete example.

## Tests

When `BUILD_TESTS=ON`, the `script_test` harness builds against the generated
wrapper for `tests/example.hpp` and runs the scripts under `tests/`:

| Script | Purpose |
|--------|---------|
| `helloworld.nut` | Minimal smoke test |
| `closure.nut` | Free-function closures and `.call()` |
| `environment.nut` | Dump root / const tables |
| `exception.nut` | Exceptions and Squirrel threads |
| `test.nut` | Table methods, varargs, classes, suspend |
| `example.nut` | Generated C++ bindings (return values, custom, suspend) |

```bash
./build/script_test tests/example.nut
./build/script_test --debug tests/test.nut
```

## Versioning

The single source of truth is the top-level `VERSION` file (e.g. `0.1.0-dev`).
Development builds append `.<revCount>+g<shortHash>`. Packaging (the Nix flake)
passes the full string into CMake as `PROJECT_VERSION_FULL`; the `miniswig`
binary reports it via `--version`.

## License

GPL-3.0-or-later (see `LICENSE.txt`). Parts of the codebase retain earlier
GPL-2.0-or-later / GPL-3.0-or-later headers from prior authors.
