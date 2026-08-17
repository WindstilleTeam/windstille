# Windstille — TODO

Living checklist for packaging, ports, and build hygiene. Prefer completing
items in order within each section; mark done with `[x]`.

Related: [PORTS.md](PORTS.md).

---

## Done (recent)

### Nix / monorepo layout
- [x] Vendor libraries under `external/` (subtrees) instead of remote flakes where possible
- [x] Shared `external/tinycmmc` modules for all subprojects
- [x] `tinycmmc_find_dependency` with multi-root / multi-name candidates and once-only `add_subdirectory`
- [x] Skip `install(EXPORT)` when packages are used as subdirectories
- [x] Guard legacy `ExportAndInstallLibrary` (wstgui / biiocpp / wstsound)
- [x] In-tree `miniswig` when the program is not on `PATH`
- [x] `argpp` **stable** branch (not master API)
- [x] `babyxml` header layout (`include/babyxml/babyxml.hpp`)
- [x] Disable `PRIO_USE_JSONCPP` by default (sexp-only)
- [x] Replace **libfmt** with `std::format` (project + surfcpp); drop `pkgs.fmt`
- [x] Rename `wstlib` → `libwindstille` (+ `wstlib` ALIAS)
- [x] Flake: separate packages for externals, `libwindstille`, `windstille`, `windstille-editor`
- [x] Flake: `apps.windstille` / `apps.windstille-editor`
- [x] Propagate CMake package deps between external flake derivations
- [x] Port scaffolding from Pingus: `mk/{wasm,android,r36s}`, `nix/{wasm,android,r36s}.nix`

### Desktop Linux build
- [x] `nix build` configures and links past the previous CMake export / missing-header failures
- [x] Argpp include path (`<argpp/argpp.hpp>`) and `std::format` align syntax (`{:<20}`)

---

## In progress / next

### Desktop quality
- [ ] Confirm clean `nix build .#windstille` and `.#windstille-editor` end-to-end (install + run smoke)
- [ ] Confirm plain CMake (no Nix) still works: `cmake -B build && cmake --build build`
- [ ] Data install / `xcf2png` path when building with split packages
- [ ] Drop remaining deprecation warnings (`cmake_minimum_required` in old subprojects)
- [ ] Optional: re-enable tests with `BUILD_TESTS` and wire `gtest` only where needed

### Flake packaging refinements
- [ ] Make `libwindstille` a true installable package consumers can link without rebuilding game sources
- [ ] Windows: finish `windstille-win64` with the full external `mkExternal` graph under `pkgsCross.mingwW64`
- [ ] Ensure each external installs a working `*-config.cmake` that `find_dependency` can load (version files, namespaces)
- [ ] Pin / document ArkOS sysroot URL + hash in `nix/r36s.nix` (replace localhost placeholder)

---

## Ports (editor not required)

See [PORTS.md](PORTS.md). Shared stack with [Pingus](https://github.com/pingus/pingus) — extend those recipes, do not fork blindly.

### Shared prerequisite: GLES2
Windstille/wstdisplay historically targeted **desktop OpenGL** (GLEW + 3.3 core). Ports need GLES2 / WebGL.

- [x] `wstdisplay/gl_compat.hpp` (Pingus-style desktop vs GLES includes)
- [x] CMake options `WINDSTILLE_USE_GLES` / `WINDSTILLE_EMSCRIPTEN` (propagated to wstdisplay)
- [x] GLES context attributes (ES 2.0 profile) and skip `glewInit` on GLES builds
- [x] Flake package `windstille-gles2` (`-DWINDSTILLE_USE_GLES=ON`) for Linux validation
- [x] Replace `gluErrorString` with portable error names
- [x] Skip fixed-function `glColor4f` on GLES
- [x] Replace `gluBuild2DMipmaps` with `glTexImage2D` + `glGenerateMipmap` on GLES
- [x] Skip VAO create/bind/delete on GLES2 (bind attributes per draw)
- [x] Guard `GL_TEXTURE_WRAP_R` and `GL_UNPACK_ROW_LENGTH` where needed
- [x] Default vertex mode away from `GL_QUADS` on GLES
- [ ] Shader / FBO path audit under pure GLES2
- [ ] Validate `nix build .#windstille-gles2` links and runs on Linux

### Windows (`nix build .#windstille-win64`)
- [ ] Cross-build all required externals for mingw
- [ ] Ship SDL2 + OpenAL + libmodplug DLLs next to the binary (Pingus flat zip pattern)
- [ ] Smoke-test under Wine / real Windows

### WebAssembly (`nix build .#windstille-wasm`)
- [ ] Wire `nix/wasm.nix` `mkApp` to Windstille sources + static deps
- [ ] Main loop: Emscripten `emscripten_set_main_loop` (or equivalent) instead of blocking desktop loop
- [ ] Asset packaging / IDBFS if needed
- [ ] Customize `mk/wasm/shell.html` branding if desired
- [ ] `nix run` / browser smoke test

### Android (`nix build .#windstille-android`)
- [ ] Flake: `androidenv.composeAndroidPackages` + license accept (Pingus pattern)
- [ ] Stage Windstille + `external/*` sources for ndk-build (or CMake-via-NDK)
- [ ] Adjust `mk/android/app/jni/Android.mk` filters (exclude editor, desktop-only code)
- [ ] Touch / gamepad mapping; lifecycle (`SDLActivity` singleTask already in manifest)
- [ ] APK via `android.mkApk` + debug keystore

### R36S / ArkOS (`nix build .#windstille-r36s`)
- [ ] Publish or document ArkOS aarch64 sysroot tarball; update hash in `nix/r36s.nix`
- [ ] Cross-build game against sysroot toolchain files in `mk/r36s/`
- [ ] PortMaster tree package (`windstille-r36s-portmaster`)
- [ ] Controller profile (`data/controller/…`) if needed
- [ ] On-device smoke test

---

## Code / engine debt (ports-related)

- [ ] Audit fixed-function / desktop-only GL calls for GLES2 viability
- [ ] Audio path on Android (OpenAL Soft static libs — see `mk/android/SOUND.md`)
- [ ] Filesystem / path assumptions (user dir, datadir) on each platform
- [ ] No editor in port builds (`BUILD_EDITOR=OFF`)

---

## Nice to have

- [ ] CI: `nix build` on Linux for default package
- [ ] CI: optional cross job for win64
- [ ] Document `nix run .#windstille` / editor in README
- [ ] Align project `version` / Android `versionName` with tags
- [ ] Consider C++23 only where `std::format` / library needs require it; keep externals consistent

---

## Notes

- **argpp** must stay on the **stable** API (`argpp::Parser`, `<argpp/argpp.hpp>`).
- **prio** JSON backend stays off unless something needs it again.
- Prefer **propagatedBuildInputs** for anything referenced from installed `*-config.cmake`.
- When in doubt, open the corresponding Pingus file under `mk/` or `nix/` and adapt — library overlap is intentional.
