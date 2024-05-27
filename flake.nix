rec {
  description = "Windstille GUI Engine";

  inputs = {
    nixpkgs.url = "github:NixOS/nixpkgs?ref=nixos-24.05";
    flake-utils.url = "github:numtide/flake-utils";

    tinycmmc.url = "github:grumbel/tinycmmc";
    tinycmmc.inputs.nixpkgs.follows = "nixpkgs";
    tinycmmc.inputs.flake-utils.follows = "flake-utils";

    argpp.url = "github:grumbel/argpp/stable";
    argpp.inputs.nixpkgs.follows = "nixpkgs";
    argpp.inputs.flake-utils.follows = "flake-utils";
    argpp.inputs.tinycmmc.follows = "tinycmmc";

    logmich.url = "github:logmich/logmich";
    logmich.inputs.nixpkgs.follows = "nixpkgs";
    logmich.inputs.tinycmmc.follows = "tinycmmc";

    geomcpp.url = "github:grumbel/geomcpp";
    geomcpp.inputs.nixpkgs.follows = "nixpkgs";
    geomcpp.inputs.tinycmmc.follows = "tinycmmc";

    priocpp.url = "github:grumbel/priocpp";
    priocpp.inputs.nixpkgs.follows = "nixpkgs";
    priocpp.inputs.flake-utils.follows = "flake-utils";
    priocpp.inputs.tinycmmc.follows = "tinycmmc";
    priocpp.inputs.logmich.follows = "logmich";

    surfcpp.url = "github:grumbel/surfcpp";
    surfcpp.inputs.nixpkgs.follows = "nixpkgs";
    surfcpp.inputs.tinycmmc.follows = "tinycmmc";
    surfcpp.inputs.geomcpp.follows = "geomcpp";
    surfcpp.inputs.logmich.follows = "logmich";
    surfcpp.inputs.SDL2-win32.follows = "SDL2-win32";

    babyxml.url = "github:grumbel/babyxml";
    babyxml.inputs.nixpkgs.follows = "nixpkgs";
    babyxml.inputs.tinycmmc.follows = "tinycmmc";

    biiocpp.url = "github:grumbel/biiocpp";
    biiocpp.inputs.nixpkgs.follows = "nixpkgs";
    biiocpp.inputs.tinycmmc.follows = "tinycmmc";

    wstinput.url = "github:WindstilleTeam/wstinput";
    wstinput.inputs.nixpkgs.follows = "nixpkgs";
    wstinput.inputs.flake-utils.follows = "flake-utils";
    wstinput.inputs.tinycmmc.follows = "tinycmmc";
    wstinput.inputs.logmich.follows = "logmich";
    wstinput.inputs.priocpp.follows = "priocpp";
    wstinput.inputs.SDL2-win32.follows = "SDL2-win32";

    wstdisplay.url = "github:WindstilleTeam/wstdisplay";
    wstdisplay.inputs.nixpkgs.follows = "nixpkgs";
    wstdisplay.inputs.tinycmmc.follows = "tinycmmc";
    wstdisplay.inputs.geomcpp.follows = "geomcpp";
    wstdisplay.inputs.babyxml.follows = "babyxml";
    wstdisplay.inputs.surfcpp.follows = "surfcpp";
    wstdisplay.inputs.logmich.follows = "logmich";
    wstdisplay.inputs.SDL2-win32.follows = "SDL2-win32";

    wstgui.url = "github:WindstilleTeam/wstgui";
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

    wstsound.url = "github:WindstilleTeam/wstsound";
    wstsound.inputs.nixpkgs.follows = "nixpkgs";
    wstsound.inputs.flake-utils.follows = "flake-utils";
    wstsound.inputs.tinycmmc.follows = "tinycmmc";

    miniswig.url = "github:WindstilleTeam/miniswig";
    miniswig.inputs.nixpkgs.follows = "nixpkgs";

    squirrel.url = "github:grumnix/squirrel";
    squirrel.inputs.nixpkgs.follows = "nixpkgs";
    squirrel.inputs.tinycmmc.follows = "tinycmmc";

    SDL2-win32.url = "github:grumnix/SDL2-win32";
    SDL2-win32.inputs.nixpkgs.follows = "nixpkgs";
    SDL2-win32.inputs.tinycmmc.follows = "tinycmmc";
  };

  outputs = { self, nixpkgs, flake-utils,
              tinycmmc, argpp, logmich, geomcpp, priocpp, surfcpp, babyxml, biiocpp,
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
            (nixpkgs.lib.optionals pkgs.targetPlatform.isWindows [
              "-DBUILD_EDITOR=OFF"
            ]);

            nativeBuildInputs = [
              pkgs.buildPackages.cmake
              pkgs.buildPackages.pkg-config
              pkgs.buildPackages.xcftools

              pkgs.buildPackages.bison
              pkgs.buildPackages.flex
            ] ++ [
              miniswig.packages.${pkgs.buildPlatform.system}.default
            ] ++
            (nixpkgs.lib.optional pkgs.targetPlatform.isLinux pkgs.makeWrapper);

            buildInputs = [
              argpp.packages.${pkgs.system}.default
              babyxml.packages.${pkgs.system}.default
              biiocpp.packages.${pkgs.system}.default
              geomcpp.packages.${pkgs.system}.default
              logmich.packages.${pkgs.system}.default
              priocpp.packages.${pkgs.system}.priocpp-sexp
              surfcpp.packages.${pkgs.system}.default
              tinycmmc.packages.${pkgs.system}.default
              wstdisplay.packages.${pkgs.system}.default
              wstgui.packages.${pkgs.system}.default
              wstinput.packages.${pkgs.system}.default
              wstsound.packages.${pkgs.system}.default

              squirrel.packages.${pkgs.system}.default

              pkgs.gtest
            ] ++
            (nixpkgs.lib.optionals pkgs.targetPlatform.isLinux [
              pkgs.gtkmm3
            ]);
          };
        };
      }
    );
}
