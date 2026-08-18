# Porting quirks and workarounds

This document records **platform-specific problems** encountered while
building Windstille for desktop Linux, GLES2, WebAssembly, Android, Windows
(MinGW), and R36S/ArkOS — and **what we changed** to make them work.

High-level packaging lives in [PORTS.md](PORTS.md). Open tasks are in
[TODO.md](TODO.md). Recipes are adapted from
[Pingus](https://github.com/pingus/pingus) (`mk/`, `nix/`).

---

## Shared themes

### Image codecs: prefer `stb_image`, not system libjpeg/libpng

**Problem.** Cross sysroots (ArkOS, Android NDK, Emscripten) often lack
usable libjpeg/libpng, or shipping static wasm copies of those libraries is
slow and fragile. Desktop flake builds previously required them for every
consumer of `surf`.

**Solution.** `surfcpp` defaults to **header-only stb_image**
(`SURF_USE_STB_IMAGE=ON`). System codecs are optional
(`SURF_USE_SYSTEM_JPEG` / `SURF_USE_SYSTEM_PNG`, off in the flake).

- Flake fetches `stb_image.h` and passes `-DSTB_IMAGE_INCLUDE_DIR=…`.
- Android / R36S / wasm use the same path.
- `surf-config.cmake.in` must **not** always `find_dependency(JPEG|PNG)`;
  only when the build actually linked those libraries (otherwise
  `wstdisplay` and friends fail configure after a clean stb-only surf).

### GLES vs desktop OpenGL

**Problem.** Ports need OpenGL ES 2.0 / WebGL; the engine historically used
desktop OpenGL + GLEW.

**Solution.** `WINDSTILLE_USE_GLES` / `WSTDISPLAY_USE_GLES`, `gl_compat.hpp`,
and a separate `windstille-gles2` package that does **not** inject a prebuilt
desktop `wstdisplay` (which would pull libGL). Fixed-function calls still need
ongoing audit for pure GLES2.

### Controller input: GameController first

**Problem.** Menus only listen to `MENU_UP/DOWN/LEFT/RIGHT` buttons, not stick
axes. Early R36S profiles mapped the D-pad to Linux joystick hat **axes 6/7**.
With `SDL_GameController`, the D-pad is **buttons 11–14**. Face buttons
worked; selection never moved. Opening a pad as GameController also emits
duplicate `JOY*`/`HAT` events, so each press was handled twice (skipped menu
entries). Binding the stick as axis-buttons with **deadzone 0** fired menu
steps on every tiny motion. Loading both `r36s.scm` and `gamepad.scm`
stacked duplicate bindings.

**Solution.**

- Profiles use the **SDL_GameControllerButton** layout (A=0… DPAD=11–14).
- Prefer GameController; **ignore** `JOYAXIS` / `JOYBUTTON` / `JOYHAT` when
  the instance is owned by a GameController.
- Pure-joystick hats still synthesize DPAD button indices 11–14.
- Do not load `gamepad.scm` as secondary when `--controller` already set a
  primary pad file.
- Stick drives movement axes only; **not** `MENU_*`.
- Axis deadzone raised from 0 to 8000.
- SDL init includes `JOYSTICK` + `GAMECONTROLLER` (VIDEO alone never delivered
  pad events).

---

## R36S / ArkOS

### Hybrid toolchain: modern GCC + old sysroot

**Problem.** A plain `pkgsCross.aarch64-multiplatform` binary links modern
nixpkgs glibc/Mesa and will not run on stock ArkOS (glibc ~2.30).

**Solution.** Cross-compile with Nix’s aarch64 GCC, but **link and take C
headers from a published ArkOS sysroot** (`nix/r36s.nix`):

| Piece | Source |
|--------|--------|
| Compiler | nixpkgs cross GCC (currently ~15) |
| C++ *headers* | same GCC (C++20 / `std::format`) |
| libc headers | ArkOS sysroot only |
| `libstdc++.so` | **ArkOS** (absolute path; `-nostdlib++`) |
| `libgcc` / `libgcc_eh` | **static** from the toolchain (`-static-libgcc`) |
| Dynamic linker | `/lib/ld-linux-aarch64.so.1` (device path) |

Wrappers (`aarch64-arkos-gcc` / `g++`) enforce include order and
`--sysroot` so GCC’s modern sys-include never defines newer glibc APIs.

### C++ exceptions abort on device

**Problem.** `throw` leads to `SIGABRT` in `uw_init_context_1` /
`_Unwind_Resume`. GCC 15 exception codegen + static `libgcc_eh` does not
match ArkOS’s older `libstdc++` unwind/personality expectations.

**Workarounds in tree today.**

- Avoid throws on non-critical paths (window icon load, some CLI parse
  errors → print + `abort`).
- `RelWithDebInfo` so on-device gdb has symbols even when unwind is broken.
- Config / argpp macros use `do { …; std::abort(); } while(0)` with a
  **terminating semicolon** (Android NDK was picky about macro expansion).

**Smoke test.** `mk/r36s/exception_test.cpp` throws/catches several exception
types. Build with the same hybrid wrappers:

```bash
nix build .#r36s-exception-test          # current cross GCC
nix build .#r36s-exception-tests         # matrix (current + gccN if available)
```

Copy `result/bin/exception_test` (or the matrix under
`share/r36s-exception-tests/`) to the device; exit 0 and `ALL PASSED` means
unwind works for that compiler. Numbered majors `14`/`13`/`12` use **pinned** nixpkgs channels
(`nixpkgs-24_11` / `24_05` / `23_11`) so the compiler is a real aarch64
*cross* GCC. Native `buildPackages.gccN` is rejected (no aarch64
`bits/c++config.h`).

**Planned follow-up.** Rebuild `windstille-r36s` with the first major that
passes the smoke test (via `overrideCC` / wrapper `gcc` pin).


### EGL window creation failed

**Problem.** `Could not create EGL window surface` when setting custom
`SDL_GL_*` attributes (color sizes, buffer size, stencil, MSAA, etc.).

**Solution.** Drop those attributes; only set profile/version if needed.
**Defaults work better** on this GLES stack than hand-tuned sizes.

### Window icon / texture fallback threw into broken unwind

**Problem.** Missing or unloadable icon/PNG paths threw through
`SoftwareSurfaceFactory::from_file` → abort in unwind.

**Solution.** Skip `set_icon` on R36S (`WINDSTILLE_R36S`); harden fallback
paths; ensure PNG/JPEG load via stb.

### PNG/JPEG missing in sysroot

**Problem.** ArkOS sysroot may not provide libpng/libjpeg for the game’s
loader.

**Solution.** Same stb_image enablement as Android (`SURF_USE_STB_IMAGE`,
header next to the build).

### Launcher script copied from Pingus

**Problem.** `Windstille.sh` passed Pingus-only CLI flags; resolution
drifted above 640×480.

**Solution.** Valid Windstille options only (`--geometry 640x480`,
`--fullscreen`, `--controller`); force 640×480 defaults and non-resizable
behavior for the handheld profile in config.

### Sysroot packaging details

- Debian-style `libc.so` linker scripts with absolute `/lib/...` paths are
  rewritten to the unpacked Nix store sysroot.
- Multiarch dir aliases (`aarch64-linux-gnu` ↔ `aarch64-unknown-linux-gnu`).
- Vendored `KHR/khrplatform.h` when the tarball is incomplete.
- Sysroot URL in the flake is still a **localhost placeholder** until a
  permanent tarball is published.

---

## WebAssembly (Emscripten)

### FreeType needs zlib at final link

**Problem.** FreeType is configured with zlib (`ftgzip`). Object objects
reference `inflateInit2_` / `inflate` / `inflateEnd`, but the final `em++`
link did not pass `-lz` → `wasm-ld` undefined symbols.

**Solution.** `mk/wasm/scripts/build-app.sh` adds `-L$ZLIB_WASM_LIBS/lib -lz`
when the static zlib prefix is present. (Configure-time `ZLIB_ROOT` alone is
not enough for the game link line.)

### No system libjpeg/libpng

**Problem.** Building static libjpeg/libpng for wasm is heavy and was still
required by old CMake `find_package` rules.

**Solution.** stb_image only; wasm build script sets
`SURF_USE_STB_IMAGE=ON` and `STB_IMAGE_INCLUDE_DIR`. Legacy jpeg/png wasm
derivations may remain in `nix/wasm.nix` but are not on the default prefix
path.

### Exceptions in the browser

**Problem.** Level loading and other paths use try/catch; without Emscripten
catch support, `___cxa_throw` aborts the runtime.

**Solution.** Link with `-fexceptions`,
`-sDISABLE_EXCEPTION_CATCHING=0`, `-sEXCEPTION_STACK_TRACES=1`.

### Black screen / TextDecoder vs growable ArrayBuffers

**Problem.** Emscripten 6.x default `GROWABLE_ARRAYBUFFERS=1` interacts badly
with `TextDecoder` on resizable buffers (startup failure paths via UTF-8
helpers).

**Solution.** `-sGROWABLE_ARRAYBUFFERS=0` so memory growth uses classic
detach-and-replace non-resizable buffers.

### GL and filesystem

- `-sFULL_ES2=1`, WebGL 1–2 range for fixed-function-style client code.
- `-sFORCE_FILESYSTEM=1`, `idbfs.js`, optional `--preload-file data@/data`.
- OpenAL when sound is enabled (`-lopenal`); modplug via static wasm lib,
  not SDL_mixer.

---

## Android

### NDK and do/while macros

**Problem.** Error macros expanded to `do { … } while(0)` **without** a
trailing semicolon after the statement form, and NDK clang reported
`expected ';' after do/while statement` in `config.cpp` / related code.

**Solution.** Ensure call sites and macro definitions are semicolon-safe
(`do { …; std::abort(); } while(0);` usage). Prefer a single
`raise_error`-style helper over ad-hoc streams.

### STL / format

**Problem.** Older NDKs lack solid `std::format`.

**Solution.** NDK **r27+** in the flake Android SDK composition; project
already moved off libfmt to `std::format`.

### Images without system PNG/JPEG

**Solution.** stb_image staged into the jni tree / include path (same as
other ports).

### Touch / pad

**Solution.** `data/controller/gamepad.scm` for SDL joystick/GameController
indices; lifecycle stays on `SDLActivity` singleTask.

---

## Desktop Linux / flake hygiene

### `nix flake check` and non-derivations

**Problem.** `packages` must be derivations only. Exporting `mkApp`,
`mkApk`, `mkInstallApp`, or string attributes (e.g. APK file name) made
`nix flake check` fail with “not a derivation”.

**Solution.** `linuxPorts.packages` holds only derivations; helpers stay on
`linuxPorts` for `apps`.

### Windows cross meta.platforms

**Problem.** MinGW cross drv has `hostPlatform = x86_64-windows` but
`meta.platforms` was set to `x86_64-linux`, so evaluation refused the
package.

**Solution.** Cross game package: `platforms = [ "x86_64-windows" ]`. Flat
`runCommand` packaging on Linux keeps Linux platforms. Win64 remains WIP
for a full external graph and is omitted from **checks**.

### surf CMake package deps

**Problem.** After switching to stb, installed `surf-config.cmake` still
required JPEG/PNG → `wstdisplay` configure failed.

**Solution.** Conditional `@SURF_CONFIG_FIND_JPEG@` / `_PNG@` substitution
at export time (see above).

---

## Windows (MinGW)

**Status.** Packaging hook and flat `exe` + DLL layout exist; linking the
full `external/*` tree under `pkgsCross.mingwW64` is still WIP. Prefer
grumnix prebuilt MinGW SDL2 / OpenAL / modplug packages over pulling
ffmpeg-heavy openal from nixpkgs cross. See [PORTS.md](PORTS.md).

---

## Quick reference: where the fixes live

| Area | Primary locations |
|------|-------------------|
| stb_image / codec policy | `external/surfcpp/CMakeLists.txt`, `surf-config.cmake.in`, `flake.nix` |
| R36S toolchain + sysroot | `nix/r36s.nix`, `mk/r36s/` |
| R36S runtime guards | `WINDSTILLE_R36S` in main / display code; `RelWithDebInfo` |
| GL attributes | `external/wstdisplay/src/opengl_window.cpp` |
| GameController / menu | `external/wstinput/`, `data/controller/{gamepad,r36s}.scm`, `windstille_main.cpp` |
| wasm link / flags | `mk/wasm/scripts/build-app.sh`, `nix/wasm.nix` |
| Android NDK / APK | `nix/android.nix`, `mk/android/` |
| Flake check surface | `flake.nix` `packages` / `checks` / `linuxPorts` |

---

## Principles that kept working

1. **Match the device ABI** (sysroot, linker, libstdc++) even if the compiler
   is newer — then narrow the compiler if exceptions still disagree.
2. **One input path** for pads (GameController); do not stack joystick + GC
   events or duplicate scm profiles.
3. **Header-only codecs** for constrained targets; keep optional system
   JPEG/PNG for desktop tools if needed later.
4. **Flake outputs that CI evaluates must be derivations**; keep functions
   under `apps` or private `let` bindings.
5. Prefer **clean defaults** (EGL attributes, resolution) over clever
   platform-specific GL/window setup unless a device proves it necessary.

When in doubt, compare with the corresponding Pingus file under `mk/` or
`nix/` and adapt rather than inventing a second stack.
