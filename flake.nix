rec {
  description = "Windstille GUI Engine";

  inputs = {
    nixpkgs.url = "github:NixOS/nixpkgs?ref=nixos-unstable";
    flake-utils.url = "github:numtide/flake-utils";

    tinycmmc.url = "git+https://github.com/grumbel/tinycmmc.git";
    tinycmmc.inputs.nixpkgs.follows = "nixpkgs";
    tinycmmc.inputs.flake-utils.follows = "flake-utils";

    argpp.url = "git+https://github.com/grumbel/argpp.git?ref=stable";
    argpp.inputs.nixpkgs.follows = "nixpkgs";
    argpp.inputs.flake-utils.follows = "flake-utils";
    argpp.inputs.tinycmmc.follows = "tinycmmc";

    logmich.url = "git+https://github.com/logmich/logmich.git";
    logmich.inputs.nixpkgs.follows = "nixpkgs";
    logmich.inputs.tinycmmc.follows = "tinycmmc";

    geomcpp.url = "git+https://github.com/grumbel/geomcpp.git";
    geomcpp.inputs.nixpkgs.follows = "nixpkgs";
    geomcpp.inputs.tinycmmc.follows = "tinycmmc";

    sexpcpp.url = "git+https://github.com/lispparser/sexp-cpp.git";
    sexpcpp.inputs.nixpkgs.follows = "nixpkgs";
    sexpcpp.inputs.flake-utils.follows = "flake-utils";
    sexpcpp.inputs.tinycmmc.follows = "tinycmmc";

    priocpp.url = "git+https://github.com/grumbel/priocpp.git";
    priocpp.inputs.nixpkgs.follows = "nixpkgs";
    priocpp.inputs.flake-utils.follows = "flake-utils";
    priocpp.inputs.tinycmmc.follows = "tinycmmc";
    priocpp.inputs.logmich.follows = "logmich";
    priocpp.inputs.sexpcpp.follows = "sexpcpp";

    surfcpp.url = "git+https://github.com/grumbel/surfcpp.git";
    surfcpp.inputs.nixpkgs.follows = "nixpkgs";
    surfcpp.inputs.tinycmmc.follows = "tinycmmc";
    surfcpp.inputs.geomcpp.follows = "geomcpp";
    surfcpp.inputs.logmich.follows = "logmich";
    surfcpp.inputs.SDL2-win32.follows = "SDL2-win32";

    babyxml.url = "git+https://github.com/grumbel/babyxml.git";
    babyxml.inputs.nixpkgs.follows = "nixpkgs";
    babyxml.inputs.tinycmmc.follows = "tinycmmc";

    biiocpp.url = "git+https://github.com/grumbel/biiocpp.git";
    biiocpp.inputs.nixpkgs.follows = "nixpkgs";
    biiocpp.inputs.tinycmmc.follows = "tinycmmc";

    wstinput.url = "git+https://github.com/WindstilleTeam/wstinput.git";
    wstinput.inputs.nixpkgs.follows = "nixpkgs";
    wstinput.inputs.flake-utils.follows = "flake-utils";
    wstinput.inputs.tinycmmc.follows = "tinycmmc";
    wstinput.inputs.logmich.follows = "logmich";
    wstinput.inputs.priocpp.follows = "priocpp";
    wstinput.inputs.SDL2-win32.follows = "SDL2-win32";
    wstinput.inputs.sexpcpp.follows = "sexpcpp";

    wstdisplay.url = "git+https://github.com/WindstilleTeam/wstdisplay.git";
    wstdisplay.inputs.nixpkgs.follows = "nixpkgs";
    wstdisplay.inputs.tinycmmc.follows = "tinycmmc";
    wstdisplay.inputs.geomcpp.follows = "geomcpp";
    wstdisplay.inputs.babyxml.follows = "babyxml";
    wstdisplay.inputs.logmich.follows = "logmich";
    wstdisplay.inputs.SDL2-win32.follows = "SDL2-win32";
    wstdisplay.inputs.surfcpp.follows = "surfcpp";

    wstgui.url = "git+https://github.com/WindstilleTeam/wstgui.git";
    wstgui.inputs.nixpkgs.follows = "nixpkgs";
    wstgui.inputs.flake-utils.follows = "flake-utils";
    wstgui.inputs.tinycmmc.follows = "tinycmmc";
    wstgui.inputs.babyxml.follows = "babyxml";
    wstgui.inputs.geomcpp.follows = "geomcpp";
    wstgui.inputs.logmich.follows = "logmich";
    wstgui.inputs.priocpp.follows = "priocpp";
    wstgui.inputs.surfcpp.follows = "surfcpp";
    wstgui.inputs.wstinput.follows = "wstinput";
    wstgui.inputs.wstdisplay.follows = "wstdisplay";
    wstgui.inputs.wstsound.follows = "wstsound";
    wstgui.inputs.sexpcpp.follows = "sexpcpp";

    wstsound.url = "git+https://github.com/WindstilleTeam/wstsound.git";
    wstsound.inputs.nixpkgs.follows = "nixpkgs";
    wstsound.inputs.flake-utils.follows = "flake-utils";
    wstsound.inputs.tinycmmc.follows = "tinycmmc";

    squirrel.url = "git+https://github.com/grumnix/squirrel.git";
    squirrel.inputs.nixpkgs.follows = "nixpkgs";
    squirrel.inputs.tinycmmc.follows = "tinycmmc";

    miniswig.url = "git+https://github.com/WindstilleTeam/miniswig.git";
    miniswig.inputs.nixpkgs.follows = "nixpkgs";
    miniswig.inputs.squirrel.follows = "squirrel";
    miniswig.inputs.tinycmmc.follows = "tinycmmc";

    SDL2-win32.url = "git+https://github.com/grumnix/SDL2-win32.git";
    SDL2-win32.inputs.nixpkgs.follows = "nixpkgs";
    SDL2-win32.inputs.tinycmmc.follows = "tinycmmc";
  };

  outputs = { self, nixpkgs, flake-utils,
              tinycmmc, argpp, logmich, geomcpp, priocpp, surfcpp, babyxml, biiocpp, sexpcpp,
              wstinput, wstdisplay, wstgui, wstsound, miniswig, squirrel, SDL2-win32 }:

    tinycmmc.lib.eachSystemWithPkgs (pkgs:
      {
        packages = rec {
          default = windstille;

          windstille = pkgs.stdenv.mkDerivation {
            pname = "windstille";
            version = "0.3.0";

            src = ./.;

            cmakeFlags = [
              "-DBUILD_EXTRA=ON"
            ] ++
            (nixpkgs.lib.optionals pkgs.stdenv.hostPlatform.isWindows [
              "-DBUILD_EDITOR=OFF"
            ]);

            nativeBuildInputs = [
              pkgs.buildPackages.cmake
              pkgs.buildPackages.pkg-config
              pkgs.buildPackages.xcftools

              pkgs.buildPackages.bison
              pkgs.buildPackages.flex
            ] ++ [
              miniswig.packages.${pkgs.stdenv.buildPlatform.system}.default
            ] ++
            (nixpkgs.lib.optional pkgs.stdenv.hostPlatform.isLinux pkgs.makeWrapper);

            buildInputs = [
              argpp.packages.${pkgs.stdenv.hostPlatform.system}.default
              babyxml.packages.${pkgs.stdenv.hostPlatform.system}.default
              biiocpp.packages.${pkgs.stdenv.hostPlatform.system}.default
              geomcpp.packages.${pkgs.stdenv.hostPlatform.system}.default
              logmich.packages.${pkgs.stdenv.hostPlatform.system}.default
              priocpp.packages.${pkgs.stdenv.hostPlatform.system}.priocpp-sexp
              surfcpp.packages.${pkgs.stdenv.hostPlatform.system}.default
              tinycmmc.packages.${pkgs.stdenv.hostPlatform.system}.default
              wstdisplay.packages.${pkgs.stdenv.hostPlatform.system}.default
              wstgui.packages.${pkgs.stdenv.hostPlatform.system}.default
              wstinput.packages.${pkgs.stdenv.hostPlatform.system}.default
              wstsound.packages.${pkgs.stdenv.hostPlatform.system}.default

              squirrel.packages.${pkgs.stdenv.hostPlatform.system}.default

              pkgs.gtest
            ] ++
            (nixpkgs.lib.optionals pkgs.stdenv.hostPlatform.isLinux [
              pkgs.gtkmm3
            ]);
          };
        };
      }
    );
}
