{
  description = "Windstille - A Sci-Fi Action-Adventure Game";

  inputs = {
    nixpkgs.url = "github:NixOS/nixpkgs?ref=nixos-unstable";
    flake-utils.url = "github:numtide/flake-utils";

    # Remote only where needed for squirrel; local external/tinycmmc is
    # used for the library helpers below.
    tinycmmc.url = "git+https://github.com/grumbel/tinycmmc.git";
    tinycmmc.inputs.nixpkgs.follows = "nixpkgs";
    tinycmmc.inputs.flake-utils.follows = "flake-utils";

    squirrel.url = "git+https://github.com/grumnix/squirrel.git";
    squirrel.inputs.nixpkgs.follows = "nixpkgs";
    squirrel.inputs.tinycmmc.follows = "tinycmmc";

    # Windows prebuilts (same stack as Pingus — avoid pkgsCross openal/ffmpeg)
    SDL2-win32.url = "git+https://github.com/grumnix/SDL2-win32.git";
    SDL2-win32.inputs.nixpkgs.follows = "nixpkgs";
    openal-soft-win32.url = "git+https://github.com/grumnix/openal-soft-win32.git";
    openal-soft-win32.inputs.nixpkgs.follows = "nixpkgs";
    libmodplug-win32.url = "git+https://github.com/grumnix/libmodplug-win32.git";
    libmodplug-win32.inputs.nixpkgs.follows = "nixpkgs";

    # Source tarballs for Android / wasm SDL stacks (shared with Pingus recipes)
    sdl2-src = {
      url = "https://github.com/libsdl-org/SDL/releases/download/release-2.30.9/SDL2-2.30.9.tar.gz";
      flake = false;
    };
    sdl2-image-src = {
      url = "https://github.com/libsdl-org/SDL_image/releases/download/release-2.8.2/SDL2_image-2.8.2.tar.gz";
      flake = false;
    };
    sdl2-mixer-src = {
      url = "https://github.com/libsdl-org/SDL_mixer/releases/download/release-2.8.0/SDL2_mixer-2.8.0.tar.gz";
      flake = false;
    };
  };

  outputs = { self, nixpkgs, flake-utils, tinycmmc, squirrel, SDL2-win32,
              openal-soft-win32, libmodplug-win32,
              sdl2-src, sdl2-image-src, sdl2-mixer-src }:
    let
      tinycmmc_lib = import ./external/tinycmmc { inherit nixpkgs flake-utils; };
    in
    tinycmmc_lib.eachSystemWithPkgs (pkgs:
      let
        inherit (pkgs) lib stdenv;

        # Shared cmake helper for external/* subtrees built as standalone
        # packages.  When CMAKE_SOURCE_DIR equals CMAKE_CURRENT_SOURCE_DIR
        # their install/export blocks run and produce *Config.cmake files
        # that tinycmmc_find_dependency() / find_package() can consume.
        mkExternal = {
          pname,
          src,
          version ? "0.0.0",
          buildInputs ? [],
          nativeBuildInputs ? [],
          cmakeFlags ? [],
          propagatedBuildInputs ? [],
        }: stdenv.mkDerivation {
          inherit pname version src cmakeFlags;
          nativeBuildInputs = [
            pkgs.buildPackages.cmake
            pkgs.buildPackages.pkg-config
          ] ++ nativeBuildInputs;
          inherit buildInputs propagatedBuildInputs;
          # Keep sources writable for out-of-tree quirks
          dontStrip = false;
        };

        # --- external libraries as separate derivations ---
        tinycmmc_pkg = mkExternal {
          pname = "tinycmmc";
          src = ./external/tinycmmc;
          version = "0.2.0";
        };

        logmich = mkExternal {
          pname = "logmich";
          src = ./external/logmich;
          version = "0.2.0";
          buildInputs = [ tinycmmc_pkg ];
          propagatedBuildInputs = [ tinycmmc_pkg ];
        };

        sexpcpp = mkExternal {
          pname = "sexpcpp";
          src = ./external/sexpcpp;
          version = "0.1.0";
          buildInputs = [ tinycmmc_pkg ];
          # Installed as CMake package "sexp"
          propagatedBuildInputs = [ tinycmmc_pkg ];
        };

        geomcpp = mkExternal {
          pname = "geomcpp";
          src = ./external/geomcpp;
          version = "0.0.0";
          buildInputs = [ pkgs.glm tinycmmc_pkg ];
          propagatedBuildInputs = [ pkgs.glm ];
        };

        babyxml = mkExternal {
          pname = "babyxml";
          src = ./external/babyxml;
          version = "0.0.0";
          buildInputs = [ tinycmmc_pkg ];
        };

        biiocpp = mkExternal {
          pname = "biiocpp";
          src = ./external/biiocpp;
          version = "0.1.0";
          buildInputs = [ tinycmmc_pkg ];
        };

        argpp = mkExternal {
          pname = "argpp";
          src = ./external/argpp;
          version = "1.0.0";
          buildInputs = [ tinycmmc_pkg ];
        };

        strutcpp = mkExternal {
          pname = "strutcpp";
          src = ./external/strutcpp;
          version = "0.0.0";
          buildInputs = [ tinycmmc_pkg ];
        };

        prio = mkExternal {
          pname = "prio";
          src = ./external/prio;
          version = "0.1.0";
          buildInputs = [ tinycmmc_pkg logmich sexpcpp ];
          # prio-config.cmake calls find_dependency(logmich) and find_dependency(sexp)
          propagatedBuildInputs = [ logmich sexpcpp ];
          cmakeFlags = [ "-DPRIO_USE_JSONCPP=OFF" ];
        };

        surfcpp = mkExternal {
          pname = "surfcpp";
          src = ./external/surfcpp;
          version = "0.0.0";
          buildInputs = [
            tinycmmc_pkg geomcpp logmich
            pkgs.libpng pkgs.libjpeg pkgs.glm
          ];
          propagatedBuildInputs = [ geomcpp logmich pkgs.libpng pkgs.libjpeg pkgs.glm ];
        };

        wstsound = mkExternal {
          pname = "wstsound";
          src = ./external/wstsound;
          version = "0.3.0";
          buildInputs = [
            tinycmmc_pkg
            pkgs.openal pkgs.libopus pkgs.opusfile
            pkgs.libogg pkgs.libvorbis pkgs.mpg123 pkgs.libmodplug
          ];
          propagatedBuildInputs = [
            pkgs.openal pkgs.libopus pkgs.opusfile
            pkgs.libogg pkgs.libvorbis pkgs.mpg123 pkgs.libmodplug
          ];
        };

        wstdisplay = mkExternal {
          pname = "wstdisplay";
          src = ./external/wstdisplay;
          version = "0.3.0";
          buildInputs = [
            tinycmmc_pkg babyxml geomcpp surfcpp logmich
            pkgs.glew pkgs.libGL pkgs.freetype pkgs.SDL2 pkgs.libsigcxx
            pkgs.sysprof
          ];
          propagatedBuildInputs = [
            babyxml geomcpp surfcpp logmich
            pkgs.glew pkgs.libGL pkgs.freetype pkgs.SDL2 pkgs.libsigcxx
          ];
        };

        # GLES2 build of wstdisplay — must not link libGL/GLEW.
        wstdisplay_gles = mkExternal {
          pname = "wstdisplay-gles";
          src = ./external/wstdisplay;
          version = "0.3.0";
          cmakeFlags = [ "-DWSTDISPLAY_USE_GLES=ON" ];
          buildInputs = [
            tinycmmc_pkg babyxml geomcpp surfcpp logmich
            pkgs.libglvnd pkgs.freetype pkgs.SDL2 pkgs.libsigcxx
            pkgs.pkg-config
          ];
          propagatedBuildInputs = [
            babyxml geomcpp surfcpp logmich
            pkgs.libglvnd pkgs.freetype pkgs.SDL2 pkgs.libsigcxx
          ];
        };

        wstinput = mkExternal {
          pname = "wstinput";
          src = ./external/wstinput;
          version = "0.3.0";
          buildInputs = [
            tinycmmc_pkg logmich prio
            pkgs.SDL2 pkgs.libsigcxx
          ];
          propagatedBuildInputs = [ logmich prio pkgs.SDL2 pkgs.libsigcxx ];
        };

        wstgui = mkExternal {
          pname = "wstgui";
          src = ./external/wstgui;
          version = "0.3.0";
          buildInputs = [
            tinycmmc_pkg sexpcpp babyxml logmich geomcpp prio surfcpp
            wstdisplay wstinput wstsound
            pkgs.SDL2 pkgs.libsigcxx pkgs.bison pkgs.flex
          ];
          propagatedBuildInputs = [
            sexpcpp babyxml logmich geomcpp prio surfcpp
            wstdisplay wstinput wstsound
            pkgs.SDL2 pkgs.libsigcxx
          ];
        };

        miniswig = mkExternal {
          pname = "miniswig";
          src = ./external/miniswig;
          version = "0.0.0";
          nativeBuildInputs = [ pkgs.bison pkgs.flex ];
        };

        windstilleExternalDeps = [
          argpp babyxml biiocpp geomcpp logmich prio surfcpp
          wstdisplay wstinput wstsound wstgui tinycmmc_pkg sexpcpp
          strutcpp
        ];

        commonNative = [
          pkgs.buildPackages.cmake
          pkgs.buildPackages.pkg-config
          pkgs.buildPackages.xcftools
          pkgs.buildPackages.bison
          pkgs.buildPackages.flex
        ];

        commonBuildInputs = windstilleExternalDeps ++ [
          miniswig
          squirrel.packages.${pkgs.stdenv.hostPlatform.system}.default
          pkgs.gtest
          pkgs.glm
          pkgs.libjpeg
          pkgs.libpng
          pkgs.glew
          pkgs.libGL
          pkgs.SDL2
          pkgs.libsigcxx
          pkgs.sysprof
          pkgs.openal
          pkgs.libopus
          pkgs.opusfile
          pkgs.libogg
          pkgs.libvorbis
          pkgs.mpg123
          pkgs.libmodplug
          pkgs.freetype
        ] ++ lib.optionals pkgs.stdenv.hostPlatform.isLinux [
          pkgs.gtkmm3
        ] ++ lib.optionals pkgs.stdenv.hostPlatform.isWindows [
          SDL2-win32.packages.${pkgs.stdenv.hostPlatform.system}.default
        ];

        # Core library.  Configures the full tree (so GenerateWrapper /
        # miniswig still run) but only builds and installs libwindstille.
        libwindstille = stdenv.mkDerivation {
          pname = "libwindstille";
          version = "0.3.0";
          src = ./.;
          cmakeFlags = [
            "-DBUILD_EDITOR=OFF"
            "-DBUILD_EXTRA=OFF"
          ];
          nativeBuildInputs = commonNative ++ [ miniswig ];
          buildInputs = commonBuildInputs;
          buildPhase = ''
            runHook preBuild
            cmake --build . --target libwindstille -j$NIX_BUILD_CORES
            runHook postBuild
          '';
          installPhase = ''
            runHook preInstall
            cmake --install . --prefix "$out"
            # Drop game binary if cmake still installed one
            rm -f "$out/bin/windstille" "$out/bin/windstille-editor" || true
            runHook postInstall
          '';
        };

        windstille = stdenv.mkDerivation {
          pname = "windstille";
          version = "0.3.0";
          src = ./.;
          cmakeFlags = [
            "-DBUILD_EDITOR=OFF"
            "-DBUILD_EXTRA=ON"
          ] ++ lib.optionals pkgs.stdenv.hostPlatform.isWindows [
            "-DBUILD_EDITOR=OFF"
          ];
          nativeBuildInputs = commonNative ++ [
            miniswig
          ] ++ lib.optional pkgs.stdenv.hostPlatform.isLinux pkgs.makeWrapper;
          buildInputs = commonBuildInputs;
        };

        windstille-editor = stdenv.mkDerivation {
          pname = "windstille-editor";
          version = "0.3.0";
          src = ./.;
          cmakeFlags = [
            "-DBUILD_EDITOR=ON"
            "-DBUILD_EXTRA=OFF"
          ];
          nativeBuildInputs = commonNative ++ [
            miniswig
          ] ++ lib.optional pkgs.stdenv.hostPlatform.isLinux pkgs.makeWrapper;
          buildInputs = commonBuildInputs ++ lib.optionals pkgs.stdenv.hostPlatform.isLinux [
            pkgs.gtkmm3
          ];
        };


        # Desktop GLES2 build to validate the embedded/WebGL path on Linux
        # (same idea as Pingus useGLES2 package). Still requires fixing
        # remaining fixed-function / desktop-only GL usage.
        # Full monorepo cmake with GLES: do not inject prebuilt desktop
        # wstdisplay (that would pull libGL).  tinycmmc_find_dependency falls
        # back to external/* and builds them with WSTDISPLAY_USE_GLES.
        windstille-gles2 = stdenv.mkDerivation {
          pname = "windstille-gles2";
          version = "0.3.0";
          src = ./.;
          cmakeFlags = [
            "-DBUILD_EDITOR=OFF"
            "-DBUILD_EXTRA=OFF"
            "-DWINDSTILLE_USE_GLES=ON"
            "-DWSTDISPLAY_USE_GLES=ON"
          ];
          nativeBuildInputs = commonNative ++ [ miniswig ]
            ++ lib.optional pkgs.stdenv.hostPlatform.isLinux pkgs.makeWrapper;
          buildInputs = [
            # No pkgs.glew / desktop wstdisplay — only GLES + shared deps.
            pkgs.libglvnd
            pkgs.pkg-config
            pkgs.freetype
            pkgs.SDL2
            pkgs.libsigcxx
            pkgs.sysprof
            pkgs.glm
            pkgs.libjpeg
            pkgs.libpng
            pkgs.openal
            pkgs.libopus
            pkgs.opusfile
            pkgs.libogg
            pkgs.libvorbis
            pkgs.mpg123
            pkgs.libmodplug
            miniswig
            squirrel.packages.${pkgs.stdenv.hostPlatform.system}.default
          ];
          # CMake installs "windstille"; expose matching name for `nix run .#windstille-gles2`.
          postInstall = ''
            if [ -e "$out/bin/windstille" ] && [ ! -e "$out/bin/windstille-gles2" ]; then
              ln -s windstille "$out/bin/windstille-gles2"
            fi
          '';
          meta = {
            description = "Windstille linked against OpenGL ES 2.0 (libGLESv2/libEGL)";
            mainProgram = "windstille";
          };
        };

        # ---- Windows cross (host Linux only) ----
        isWin = pkgs.stdenv.hostPlatform.isWindows;
        win64Pkgs = if isWin then null else pkgs.pkgsCross.mingwW64;

        # Prebuilt MinGW runtime DLLs (grumnix) — avoid pkgsCross openal→ffmpeg.
        sdl2Win64 = SDL2-win32.packages.${pkgs.system}.SDL2-win64;
        openalWin64 = openal-soft-win32.packages.${pkgs.system}.openal-soft-win64;
        modplugWin64 = libmodplug-win32.packages.${pkgs.system}.libmodplug-win64;

        windstille-win64-game =
          if isWin || win64Pkgs == null then null
          else win64Pkgs.stdenv.mkDerivation {
            pname = "windstille";
            version = "0.3.0";
            src = ./.;
            cmakeFlags = [
              "-DBUILD_EDITOR=OFF"
              "-DBUILD_EXTRA=OFF"
              "-DWINDSTILLE_USE_GLES=OFF"
              "-DPRIO_USE_JSONCPP=OFF"
            ];
            nativeBuildInputs = [
              win64Pkgs.buildPackages.cmake
              win64Pkgs.buildPackages.pkg-config
              win64Pkgs.buildPackages.bison
              win64Pkgs.buildPackages.flex
            ];
            buildInputs = [
              sdl2Win64
              openalWin64
              modplugWin64
            ];
            # Full external/* cmake graph for MinGW is still WIP (see PORTS.md).
            # This derivation is the packaging hook; link failures are expected
            # until mkExternal is exercised under pkgsCross.mingwW64.
            meta = {
              description = "Windstille Windows x86_64 game binary (mingwW64) — WIP";
              platforms = [ "x86_64-linux" ];
            };
          };

        # Flat layout: .exe + DLLs + data/ (Pingus-style zip-friendly tree).
        windstille-win64 =
          if windstille-win64-game == null then null
          else pkgs.runCommand "windstille-win64" {
            meta = {
              description = "Windstille Windows x86_64 flat package (exe + DLLs)";
              platforms = [ "x86_64-linux" ];
            };
          } ''
            mkdir -p $out
            # Game binary (when the cross link succeeds)
            if [ -d "${windstille-win64-game}/bin" ]; then
              cp -v ${windstille-win64-game}/bin/*.exe $out/ 2>/dev/null || true
              cp -vL ${windstille-win64-game}/bin/*.dll $out/ 2>/dev/null || true
            fi
            # Runtime DLLs from prebuilt MinGW packages
            for pkg in ${sdl2Win64} ${openalWin64} ${modplugWin64}; do
              find "$pkg" -name '*.dll' -exec cp -v {} $out/ \; 2>/dev/null || true
            done
            # Data next to the binary (relative "data/" path)
            if [ -d ${./data} ]; then
              mkdir -p $out/data
              cp -a ${./data}/. $out/data/ || true
            elif [ -d "${windstille-win64-game}/share" ]; then
              cp -a ${windstille-win64-game}/share/windstille/. $out/data/ 2>/dev/null || true
            fi
            # Placeholder so the package always exists while cross-link is WIP
            if ! ls $out/*.exe >/dev/null 2>&1; then
              echo "windstille-win64: game binary not built yet (externals WIP)" > $out/README-WIP.txt
            fi
            ls -la $out || true
          '';

        # ---- Linux-only: WASM / Android / R36S (Pingus-derived recipes) ----
        # Full APK/sysroot wiring needs SDK license accept + published ArkOS
        # sysroot; helper stacks are importable now.  See PORTS.md.
        linuxPorts =
          if !pkgs.stdenv.hostPlatform.isLinux then {}
          else
            let
              wasm = import ./nix/wasm.nix {
                inherit pkgs;
                sdlSrc = sdl2-src;
                sdlVersion = "2.30.9";
                sdlImageSrc = sdl2-image-src;
                # No SDL2_mixer: Windstille audio is OpenAL Soft + libmodplug
                # (see nix/wasm.nix modplugWasm / enableSound).
                sdlMixerSrc = null;
                squirrelSrc = squirrel;
              };
              # Data is large; preload only if present.  Empty dataDir still
              # produces a linkable .html/.js/.wasm for smoke tests.
              wasmDataDir = if builtins.pathExists ./data then ./data else null;
              windstille-wasm = wasm.mkApp {
                appName = "windstille";
                srcDir = ./.;
                dataDir = wasmDataDir;
                enableSound = true;
                enableGles2 = true;
                enableAsyncify = false;
                versionFull = "0.3.0-dev";
                gitRev = self.rev or "dirty";
                sourceUrl = "https://github.com/WindstilleTeam/windstille";
              };

              # Android SDK (license accept + unfree) — same pattern as Pingus.
              androidPkgs = import nixpkgs {
                system = pkgs.stdenv.hostPlatform.system;
                config.allowUnfree = true;
                config.android_sdk.accept_license = true;
              };
              buildToolsVersion = "30.0.3";
              packagePlatform = "22";
              compilePlatform = "33";
              # NDK with solid std::format support (r27+).
              ndkVersion = "27.0.12077973";
              targetAbis = [ "armeabi-v7a" "arm64-v8a" ];
              androidSdk = (androidPkgs.androidenv.composeAndroidPackages {
                platformVersions = [ packagePlatform compilePlatform ];
                buildToolsVersions = [ buildToolsVersion ];
                includeNDK = true;
                inherit ndkVersion;
                includeEmulator = false;
                includeSources = false;
              }).androidsdk;

              android = import ./nix/android.nix {
                pkgs = androidPkgs;
                sdlSrc = sdl2-src;
                sdlVersion = "2.30.9";
                # Mixer optional; Windstille uses OpenAL Soft + modplug via audioAndroidLibs.
                inherit androidSdk buildToolsVersion packagePlatform compilePlatform targetAbis;
                squirrelSrc = squirrel;
              };

              gitDate =
                if self ? lastModifiedDate then builtins.substring 0 8 self.lastModifiedDate
                else "00000000";
              gitRevShort = self.rev or "dirty";
              androidApkName = "windstille-${gitDate}-${builtins.substring 0 7 gitRevShort}.apk";
              stbImageH = androidPkgs.fetchurl {
                url = "https://raw.githubusercontent.com/nothings/stb/refs/heads/master/stb_image.h";
                sha256 = "sha256-WUwv411JSItDgtv67I+YNm3vyoGdkWrJW+zz519CALM=";
              };

              windstille-android = android.mkApk {
                appName = "windstille";
                appDir = ./mk/android/app;
                outApkName = androidApkName;
                keystore = ./mk/android/keystore/debug.keystore;
                gameSrcDir = ./src;
                gameExternalDir = ./external;
                glmIncludeDir = "${androidPkgs.glm}/include";
                gameDataDir = if builtins.pathExists ./data then ./data else null;
                inherit stbImageH;
                gameVersion = "0.3.0-dev";
              };

              # R36S / ArkOS — sysroot URL is still a localhost placeholder in
              # nix/r36s.nix; update hash when a permanent tarball is published.
              r36s = import ./nix/r36s.nix {
                inherit (pkgs) lib stdenv stdenvNoCC fetchurl cmake pkg-config writeShellScript zip glm;
                pkgsCross = pkgs.pkgsCross;
              };
              windstille-r36s = r36s.mkWindstilleR36s {
                src = ./.;
                version = "0.3.0-dev";
                pname = "windstille-r36s";
                enableSound = true;
              };
              windstille-r36s-portmaster = r36s.mkWindstilleR36sPortMaster {
                r36sPkg = windstille-r36s;
                version = "0.3.0-dev";
                pname = "windstille-r36s-portmaster";
              };
              windstille-r36s-portmaster-zip = r36s.mkWindstilleR36sPortMasterZip {
                portMasterPkg = windstille-r36s-portmaster;
                version = "0.3.0-dev";
                pname = "windstille-r36s-portmaster-zip";
              };
            in {
              inherit (wasm) sdl2WasmLibs zlibWasmLibs sdlWasmLibs
                mkApp mkOpenBrowserApp glmPrefix sigcWasm;
              inherit windstille-wasm;
              windstille-wasm-helpers = wasm.sdl2WasmLibs;
              inherit (android) sdlAndroidLibs mkApk mkInstallApp;
              inherit windstille-android;
              android-sdl-libs = android.sdlAndroidLibs;
              inherit androidApkName;
              inherit (r36s) arkosSysroot;
              inherit windstille-r36s windstille-r36s-portmaster windstille-r36s-portmaster-zip;
            };

      in
      {
        packages = rec {
          inherit
            tinycmmc_pkg logmich sexpcpp geomcpp babyxml biiocpp argpp
            strutcpp prio surfcpp wstsound wstdisplay wstdisplay_gles wstinput wstgui
            miniswig
            libwindstille
            windstille
            windstille-editor
            windstille-gles2;
          tinycmmc = tinycmmc_pkg;
          default = windstille;
        } // lib.optionalAttrs (windstille-win64 != null) {
          inherit windstille-win64;
        } // linuxPorts;

        apps = {
          windstille = {
            type = "app";
            program = "${windstille}/bin/windstille";
          };
          windstille-editor = {
            type = "app";
            program = "${windstille-editor}/bin/windstille-editor";
          };
          windstille-gles2 = {
            type = "app";
            program = "${windstille-gles2}/bin/windstille";
          };
          default = {
            type = "app";
            program = "${windstille}/bin/windstille";
          };
        } // lib.optionalAttrs (pkgs.stdenv.hostPlatform.isLinux && linuxPorts ? windstille-wasm) {
          windstille-wasm = (import ./nix/wasm.nix {
            inherit pkgs;
            sdlSrc = sdl2-src;
            sdlVersion = "2.30.9";
            sdlImageSrc = sdl2-image-src;
            sdlMixerSrc = null; # Windstille: OpenAL + modplug, not SDL_mixer
            squirrelSrc = squirrel;
          }).mkOpenBrowserApp {
            pkg = linuxPorts.windstille-wasm;
            appName = "windstille";
            description = "Serve and open the Windstille wasm build in a browser";
          };
        } // lib.optionalAttrs (pkgs.stdenv.hostPlatform.isLinux && linuxPorts ? windstille-android) {
          install-android-windstille = linuxPorts.mkInstallApp {
            pkg = linuxPorts.windstille-android;
            apkFileName = linuxPorts.androidApkName;
            description = "Install Windstille APK to a connected Android device via adb";
          };
        } // lib.optionalAttrs (windstille-win64 != null && pkgs.stdenv.hostPlatform.isLinux) {
          windstille-win64 = {
            type = "app";
            program = toString (pkgs.writeShellScript "windstille-win64-wine" ''
              set -euo pipefail
              export WINEPREFIX=$(mktemp -d)
              export WINEARCH=win64
              export WINEDLLOVERRIDES="mscoree,mshtml=;SDL2=n"
              cd ${windstille-win64}
              exe=$(ls *.exe 2>/dev/null | head -1 || true)
              if [ -z "$exe" ]; then
                echo "windstille-win64: no .exe yet (cross-link still WIP)" >&2
                exit 1
              fi
              exec ${pkgs.wineWow64Packages.stable}/bin/wine "./$exe" "$@"
            '');
            meta.description = "Windstille (MinGW x86_64) via Wine — WIP until externals cross-build";
          };
        };
      }
    );
}
