{
  description = "Software Surface Library";

  inputs = {
    nixpkgs.url = "github:NixOS/nixpkgs?ref=nixos-unstable";

    tinycmmc.url = "github:grumbel/tinycmmc";
    tinycmmc.inputs.nixpkgs.follows = "nixpkgs";

    logmich.url = "github:logmich/logmich";
    logmich.inputs.nixpkgs.follows = "nixpkgs";
    logmich.inputs.tinycmmc.follows = "tinycmmc";

    geomcpp.url = "github:grumbel/geomcpp";
    geomcpp.inputs.nixpkgs.follows = "nixpkgs";
    geomcpp.inputs.tinycmmc.follows = "tinycmmc";

    SDL2-win32.url = "github:grumnix/SDL2-win32";
  };

  outputs = { self, nixpkgs, tinycmmc, geomcpp, logmich, SDL2-win32 }:
    tinycmmc.lib.eachSystemWithPkgs (pkgs:
      {
        packages = rec {
          default = surfcpp;

          surfcpp = pkgs.callPackage ./surfcpp.nix {
            stdenv = pkgs.stdenv;
            SDL2 = if pkgs.stdenv.targetPlatform.isWindows then SDL2-win32.packages.${pkgs.stdenv.hostPlatform.system}.default else pkgs.SDL2;
            libjpeg = if pkgs.stdenv.targetPlatform.isWindows
                      then (pkgs.libjpeg_original.overrideAttrs (oldAttrs: { meta = {}; }))
                      else pkgs.libjpeg;
            geomcpp = geomcpp.packages.${pkgs.stdenv.targetPlatform.system}.default;
            logmich = logmich.packages.${pkgs.stdenv.targetPlatform.system}.default;
            tinycmmc = tinycmmc.packages.${pkgs.stdenv.targetPlatform.system}.default;
          };
        };
      }
    );
}
