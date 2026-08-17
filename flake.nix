{
  description = "Binding generator for Squirrel";

  inputs = {
    nixpkgs.url = "github:NixOS/nixpkgs?ref=nixos-unstable";

    tinycmmc.url = "github:grumbel/tinycmmc";
    tinycmmc.inputs.nixpkgs.follows = "nixpkgs";

    squirrel.url = "github:grumnix/squirrel";
    squirrel.inputs.nixpkgs.follows = "nixpkgs";
    squirrel.inputs.tinycmmc.follows = "tinycmmc";
  };

  outputs = { self, nixpkgs, tinycmmc, squirrel }:
    tinycmmc.lib.eachSystemWithPkgs (pkgs:
      {
        packages = rec {
          default = miniswig;

          miniswig = pkgs.callPackage ./miniswig.nix {
            squirrel = squirrel.packages.${pkgs.stdenv.hostPlatform.system}.default;
          };
        };
      }
    );
}
