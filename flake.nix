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

    SDL2-win32.url = "git+https://github.com/grumnix/SDL2-win32.git";
    SDL2-win32.inputs.nixpkgs.follows = "nixpkgs";
  };

  outputs = { self, nixpkgs, flake-utils, tinycmmc, squirrel, SDL2-win32 }:
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
        };

        sexpcpp = mkExternal {
          pname = "sexpcpp";
          src = ./external/sexpcpp;
          version = "0.1.0";
          buildInputs = [ tinycmmc_pkg ];
        };

        geomcpp = mkExternal {
          pname = "geomcpp";
          src = ./external/geomcpp;
          version = "0.0.0";
          buildInputs = [ pkgs.glm tinycmmc_pkg ];
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
        };

        wstinput = mkExternal {
          pname = "wstinput";
          src = ./external/wstinput;
          version = "0.3.0";
          buildInputs = [
            tinycmmc_pkg logmich prio
            pkgs.SDL2 pkgs.libsigcxx
          ];
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
      in
      {
        packages = rec {
          inherit
            tinycmmc_pkg logmich sexpcpp geomcpp babyxml biiocpp argpp
            strutcpp prio surfcpp wstsound wstdisplay wstinput wstgui
            miniswig
            libwindstille
            windstille
            windstille-editor;
          tinycmmc = tinycmmc_pkg;
          default = windstille;
        };

        apps = {
          windstille = {
            type = "app";
            program = "${windstille}/bin/windstille";
          };
          windstille-editor = {
            type = "app";
            program = "${windstille-editor}/bin/windstille-editor";
          };
          default = {
            type = "app";
            program = "${windstille}/bin/windstille";
          };
        };
      }
    );
}
