# Cross-platform ports

Port packaging is based on the [Pingus](https://github.com/pingus/pingus) tree
(`mk/` scripts + `nix/` glue). Prefer studying and extending those recipes over
rewriting them: SDL2, OpenAL, libmodplug, GLES, and Emscripten constraints are
already solved there.

| Target | Preferred command | Notes |
|--------|-------------------|--------|
| **Windows** | `nix build .#windstille-win64` | `pkgsCross.mingwW64` + grumnix SDL2/OpenAL |
| **WebAssembly** | `nix build .#windstille-wasm` | Emscripten; shell in `mk/wasm/shell.html` |
| **Android** | `nix build .#windstille-android` | NDK + `mk/android/app` |
| **R36S / ArkOS** | `nix build .#windstille-r36s` | Sysroot + PortMaster zip |

Editor is **not** built for these targets (`BUILD_EDITOR=OFF`).

## Layout

- `mk/wasm/` — Emscripten shell, SDL static-build scripts, `serve.sh`
- `mk/android/` — Manifest, jni/`Android.mk`, keystore, APK scripts
- `mk/r36s/` — ArkOS toolchains, cxxabi shim, sysroot helpers
- `nix/wasm.nix`, `nix/android.nix`, `nix/r36s.nix` — Nix packaging

## GLES / embedded GL

Windstille currently targets desktop OpenGL via `wstdisplay`. Handheld and
mobile ports need a GLES2 path (Pingus uses `PINGUS_USE_GLES` /
`opengl_compat.hpp`). Expect follow-up work in `external/wstdisplay` and the
game before Android/R36S/WASM run beyond a smoke link.

## Without Nix

Scripts under `mk/*/scripts/` can be driven with a normal SDK/NDK/Emscripten
install; see each directory’s README.
