rec {
  description = "Windstille GUI Engine";

  inputs = {
    nixpkgs.url = "github:NixOS/nixpkgs?ref=nixos-unstable";
    flake-utils.url = "github:numtide/flake-utils";

    tinycmmc.url = "git+https://github.com/grumbel/tinycmmc.git";
    tinycmmc.inputs.nixpkgs.follows = "nixpkgs";
    tinycmmc.inputs.flake-utils.follows = "flake-utils";

    logmich.url = "git+https://github.com/logmich/logmich.git";
    logmich.inputs.nixpkgs.follows = "nixpkgs";
    logmich.inputs.tinycmmc.follows = "tinycmmc";

    geomcpp.url = "git+https://github.com/grumbel/geomcpp.git";
    geomcpp.inputs.nixpkgs.follows = "nixpkgs";
    geomcpp.inputs.tinycmmc.follows = "tinycmmc";

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

    babyxml.url = "git+https://github.com/grumbel/babyxml.git";
    babyxml.inputs.nixpkgs.follows = "nixpkgs";
    babyxml.inputs.tinycmmc.follows = "tinycmmc";

    sexpcpp.url = "git+https://github.com/lispparser/sexp-cpp.git";
    sexpcpp.inputs.nixpkgs.follows = "nixpkgs";
    sexpcpp.inputs.flake-utils.follows = "flake-utils";
    sexpcpp.inputs.tinycmmc.follows = "tinycmmc";

    wstinput.url = "git+https://github.com/windstilleteam/wstinput.git";
    wstinput.inputs.nixpkgs.follows = "nixpkgs";
    wstinput.inputs.flake-utils.follows = "flake-utils";
    wstinput.inputs.tinycmmc.follows = "tinycmmc";
    wstinput.inputs.logmich.follows = "logmich";
    wstinput.inputs.priocpp.follows = "priocpp";
    wstinput.inputs.sexpcpp.follows = "sexpcpp";

    wstdisplay.url = "git+https://github.com/windstilleteam/wstdisplay.git";
    wstdisplay.inputs.nixpkgs.follows = "nixpkgs";
    wstdisplay.inputs.tinycmmc.follows = "tinycmmc";
    wstdisplay.inputs.geomcpp.follows = "geomcpp";
    wstdisplay.inputs.babyxml.follows = "babyxml";
    wstdisplay.inputs.surfcpp.follows = "surfcpp";
    wstdisplay.inputs.logmich.follows = "logmich";

    wstsound.url = "git+https://github.com/windstilleteam/wstsound.git";
    wstsound.inputs.nixpkgs.follows = "nixpkgs";
    wstsound.inputs.flake-utils.follows = "flake-utils";
    wstsound.inputs.tinycmmc.follows = "tinycmmc";
  };

  outputs = { self, nixpkgs, flake-utils,
              tinycmmc, logmich, geomcpp, priocpp, surfcpp, babyxml, sexpcpp,
              wstinput, wstdisplay, wstsound }:

    tinycmmc.lib.eachSystemWithPkgs (pkgs:
      {
        packages = rec {
          default = wstgui;

          wstgui = pkgs.stdenv.mkDerivation {
            pname = "wstgui";
            version = "0.3.0";

            src = nixpkgs.lib.cleanSource ./.;

            cmakeFlags = [ "-DBUILD_EXTRA=ON" ];

            nativeBuildInputs = [
              pkgs.buildPackages.cmake
              pkgs.buildPackages.pkg-config
            ];

            postFixup =
              (nixpkgs.lib.optionalString pkgs.stdenv.hostPlatform.isWindows ''
                mkdir -p $out/bin/
                ln -sfv ${wstdisplay.packages.${pkgs.stdenv.hostPlatform.system}.default}/bin/*.dll $out/bin/

                # FIXME: should be handled by sexpcpp itself
                ln -sfv ${pkgs.jsoncpp}/bin/*.dll $out/bin/
               '');

            buildInputs = [
              babyxml.packages.${pkgs.stdenv.hostPlatform.system}.default
            ];

            propagatedBuildInputs = [
              (geomcpp.packages.${pkgs.stdenv.hostPlatform.system}.default.override { })
              (logmich.packages.${pkgs.stdenv.hostPlatform.system}.default.overrideAttrs (old: { name = "lutsch"; }))
              priocpp.packages.${pkgs.stdenv.hostPlatform.system}.default
              sexpcpp.packages.${pkgs.stdenv.hostPlatform.system}.default
              surfcpp.packages.${pkgs.stdenv.hostPlatform.system}.default
              tinycmmc.packages.${pkgs.stdenv.hostPlatform.system}.default
              wstdisplay.packages.${pkgs.stdenv.hostPlatform.system}.default
              wstinput.packages.${pkgs.stdenv.hostPlatform.system}.default
              wstsound.packages.${pkgs.stdenv.hostPlatform.system}.default
            ];
           };
        };
      }
    );
}
