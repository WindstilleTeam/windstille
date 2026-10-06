# Cross-platform ports

Port packaging is based on the [Pingus](https://github.com/pingus/pingus) tree
(`mk/` scripts + `nix/` glue). Prefer studying and extending those recipes over
rewriting them: SDL2, OpenAL, libmodplug, GLES, and Emscripten constraints are
already solved there.

| Target | Preferred command | Status |
|--------|-------------------|--------|
| **Linux (desktop)** | `nix build .#windstille` | Primary target |
| **Linux GLES2** | `nix build .#windstille-gles2` | Validates embedded GL path |
| **Windows** | `nix build .#windstille-win64` | MinGW-w64; flat exe+DLL layout, `nix run` via Wine |
| **WebAssembly** | `nix build .#windstille-wasm` | Emscripten; shell in `mk/wasm/shell.html` |
| **Android** | `nix build .#windstille-android` | NDK + `mk/android/app`; APK via debug keystore |
| **R36S / ArkOS** | `nix build .#windstille-r36s` | Sysroot + PortMaster zip (sysroot URL placeholder) |

Editor is **not** built for port targets (`BUILD_EDITOR=OFF`).

## Layout

- `mk/wasm/` — Emscripten shell, SDL static-build scripts, `serve.sh`
- `mk/android/` — Manifest, jni/`Android.mk`, keystore, APK scripts
- `mk/r36s/` — ArkOS toolchains, cxxabi shim, sysroot helpers, PortMaster metadata
- `nix/wasm.nix`, `nix/android.nix`, `nix/r36s.nix`, `nix/win64.nix` — Nix packaging

All ports build the vendored libraries in `external/` (including
`external/wst`) as part of the game instead of as separate packages.

## GLES / embedded GL

wst renders through GLES 2.0 as its baseline and picks GL 3.3 core or
GLES 2.0 at runtime, GL entry points are loaded through SDL with GLAD. No
build flags are needed for GLES; `WINDSTILLE_USE_GLES` only makes the game
request a GLES context.

## Windows notes

- Prebuilt MinGW SDL2 / OpenAL Soft / libmodplug come from grumnix flakes
  (avoids pkgsCross openal → ffmpeg).
- Flat package copies runtime DLLs next to the `.exe` and ships `data/`.
- squirrel is cross-built as a static library, everything in `external/` is
  built as part of the game.
- The package collects the DLLs the `.exe` needs by following its imports.

## WebAssembly notes

- `emscripten_set_main_loop` drives `wstgui::ScreenManager::run_frame`
- Datadir `/data/`, userdir `/windstille-user/`
- Assets via `--preload-file`; optional IDBFS for saves (shell backup UI exists)

## Android notes

- Package `org.windstille.game`, `SDLActivity` with `singleTask`
- Empty datadir (AssetManager); userdir from `SDL_AndroidGetInternalStoragePath`
- Sound via OpenAL Soft + libmodplug when `AUDIO_ANDROID_LIBS` is staged

## Controller profiles

- `data/controller/keyboard.scm` — default desktop
- `data/controller/xboxdrv.scm` — Xbox pad via xpad
- `data/controller/gamepad.scm` — generic SDL joystick (Android / handhelds)
- `data/controller/r36s.scm` — R36S / ArkOS built-in pad

Load with `--controller data/controller/r36s.scm` (or set `primary-controller-file` in config).

## R36S / ArkOS notes

- Link against the published ArkOS aarch64 sysroot (glibc ~2.30), not modern
  nixpkgs glibc — see `mk/r36s/CROSSCOMPILE.md`
- Sysroot fetch URL in `nix/r36s.nix` is still a localhost placeholder, so
  R36S is not part of `nix flake check`
- PortMaster tree + zip packages are ready once the sysroot builds

## Without Nix

Scripts under `mk/*/scripts/` can be driven with a normal SDK/NDK/Emscripten
install; see each directory’s README.
