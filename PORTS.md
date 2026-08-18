# Cross-platform ports

Port packaging is based on the [Pingus](https://github.com/pingus/pingus) tree
(`mk/` scripts + `nix/` glue). Prefer studying and extending those recipes over
rewriting them: SDL2, OpenAL, libmodplug, GLES, and Emscripten constraints are
already solved there.

| Target | Preferred command | Status |
|--------|-------------------|--------|
| **Linux (desktop)** | `nix build .#windstille` | Primary target |
| **Linux GLES2** | `nix build .#windstille-gles2` | Validates embedded GL path |
| **Windows** | `nix build .#windstille-win64` | Flat exe+DLL layout; MinGW externals still WIP |
| **WebAssembly** | `nix build .#windstille-wasm` | Emscripten; shell in `mk/wasm/shell.html` |
| **Android** | `nix build .#windstille-android` | NDK + `mk/android/app`; APK via debug keystore |
| **R36S / ArkOS** | `nix build .#windstille-r36s` | Sysroot + PortMaster zip (sysroot URL placeholder) |

Editor is **not** built for port targets (`BUILD_EDITOR=OFF`).

## Layout

- `mk/wasm/` — Emscripten shell, SDL static-build scripts, `serve.sh`
- `mk/android/` — Manifest, jni/`Android.mk`, keystore, APK scripts
- `mk/r36s/` — ArkOS toolchains, cxxabi shim, sysroot helpers, PortMaster metadata
- `nix/wasm.nix`, `nix/android.nix`, `nix/r36s.nix` — Nix packaging

## GLES / embedded GL

Much of the GLES2 path is in place (`WINDSTILLE_USE_GLES`, ES shaders,
FBO/MSAA guards, main-loop for Emscripten). Remaining work:

- Effect shaders that still use fixed-pipeline varyings (`gl_TexCoord`)
- Replace some blit callers with a textured-quad path on GLES2
- Runtime smoke under mesa llvmpipe / real GLES / WebGL

## Windows notes

- Prebuilt MinGW SDL2 / OpenAL Soft / libmodplug come from grumnix flakes
  (avoids pkgsCross openal → ffmpeg).
- Flat package copies runtime DLLs next to the `.exe` and ships `data/`.
- Full `external/*` cmake packages under `pkgsCross.mingwW64` are not wired yet;
  the derivation is the packaging hook until that lands.

## WebAssembly notes

- `emscripten_set_main_loop` drives `ScreenManager::run_one_frame`
- Datadir `/data/`, userdir `/windstille-user/`
- Assets via `--preload-file`; optional IDBFS for saves (shell backup UI exists)

## Android notes

- Package `org.windstille.game`, `SDLActivity` with `singleTask`
- Empty datadir (AssetManager); userdir from `SDL_AndroidGetInternalStoragePath`
- Sound via OpenAL Soft + libmodplug when `AUDIO_ANDROID_LIBS` is staged

## R36S / ArkOS notes

- Link against the published ArkOS aarch64 sysroot (glibc ~2.30), not modern
  nixpkgs glibc — see `mk/r36s/CROSSCOMPILE.md`
- Sysroot fetch URL in `nix/r36s.nix` is still a localhost placeholder
- PortMaster tree + zip packages are ready once the sysroot builds

## Without Nix

Scripts under `mk/*/scripts/` can be driven with a normal SDK/NDK/Emscripten
install; see each directory’s README.
