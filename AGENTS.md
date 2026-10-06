# AGENTS.md

Guidance for automated agents working on this repository.

## Project overview

miniswig is a small C++ tool that generates Squirrel language bindings from
preprocessed C++ headers. The core is a flex/bison parser (`src/lexer.ll`,
`src/parser.yy`) plus tree walkers that emit wrapper code and optional docs.

## Layout

| Path | Role |
|------|------|
| `src/` | Compiler: lexer, parser, AST (`tree.*`), codegen (`create_wrapper.*`, `create_docu.*`), `main.cpp` |
| `include/squirrel/` | Small helpers used by tests / embeds (e.g. `squirrel_error.hpp`) |
| `tests/` | Example API (`example.hpp` / `example.cpp`), generated-wrapper integration via `script_test`, and `.nut` scripts |
| `VERSION` | **Only** source of truth for the version string |
| `flake.nix` / `miniswig.nix` | Nix packaging; appends git rev info for `-dev` builds |
| `CMakeLists.txt` | Build, optional tests (`BUILD_TESTS`) |

## Versioning rules

- Edit `VERSION` only (e.g. `0.1.0-dev`). Do not duplicate the version elsewhere.
- Inside git, keep the `-dev` suffix. Packaging may pass
  `-DPROJECT_VERSION_FULL=...` with an extended development form
  (`0.1.0-dev.1615+gf1fb306`).
- Release process: strip `-dev` / metadata from `VERSION`, commit, tag `vX.Y.Z`
  matching the file (with a `v` prefix).

## Build & test

```bash
cmake -B build -DBUILD_TESTS=ON
cmake --build build
ctest --test-dir build --output-on-failure
```

Nix: `nix build`, `nix develop`. Prefer the Nix flake for dependency versions
(Squirrel, flex, bison).

Tests require Squirrel 3.2 and compile a generated wrapper for
`tests/example.hpp`. New language-level scripts go in `tests/*.nut` and should
be listed in the `SCRIPT_TESTS` set in `CMakeLists.txt`.

## Coding conventions

- C++: match existing style in `src/` (relatively compact, no heavy frameworks).
- Keep commits small and task-focused.
- Author line for commits in this project: `Ingo Ruhnke <grumbel@gmail.com>`
  with trailer `Co-authored-by: Grok <grok@x.ai>` when an agent co-authors.
- Do not reintroduce `tinycmmc` as a direct packaging dependency; version and
  multi-system helpers live in `flake.nix` / `VERSION`.
- Do not add a `.gitattributes` export-subst for `VERSION`; the file is plain
  text.

## Binding annotations

When extending the example API or documenting headers for miniswig:

- `__suspend` on functions that should call `sq_suspendvm`.
- `__custom("paramscheck")` on hand-written `SQInteger(HSQUIRRELVM)` functions.

The generator only sees preprocessed input (`-E -CC`); macros must expand
appropriately with/without `SCRIPTING_API`.

## What not to do

- Do not commit generated `parser.cpp` / `lexer.cpp` or `example_wrap.*`.
- Do not bump version in `miniswig.nix` or CMake independently of `VERSION`.
- Do not expand scope into a full SWIG replacement; keep the tool focused on
  the Squirrel subset this codebase already supports.
