{
  description = "Binding generator for Squirrel";

  inputs = {
    nixpkgs.url = "github:NixOS/nixpkgs?ref=nixos-unstable";
    flake-utils.url = "github:numtide/flake-utils";

    tinycmmc.url = "github:grumbel/tinycmmc";
    tinycmmc.inputs.nixpkgs.follows = "nixpkgs";

    squirrel.url = "github:grumnix/squirrel";
    squirrel.inputs.nixpkgs.follows = "nixpkgs";
    squirrel.inputs.tinycmmc.follows = "tinycmmc";
  };

  outputs = { self, nixpkgs, flake-utils, tinycmmc, squirrel }:
    flake-utils.lib.eachDefaultSystem (system:
      let
        pkgs = nixpkgs.legacyPackages.${system};
        lib = pkgs.lib;
        versionBase = lib.strings.removeSuffix "\n" (builtins.readFile ./VERSION);
        gitRev = "${self.shortRev or self.dirtyShortRev or "dirty"}";
        # Development builds append .<revCount>+g<shortHash> (and -dirty when needed).
        # Release builds (VERSION without -dev) use the base version as-is.
        version =
          if lib.strings.hasInfix "-dev" versionBase then
            "${versionBase}.${toString (self.revCount or 0)}+g${gitRev}"
          else
            versionBase;
      in {
        packages = rec {
          default = miniswig;

          miniswig = pkgs.callPackage ./miniswig.nix {
            inherit version;
            squirrel = squirrel.packages.${system}.default;
          };
        };
      }
    );
}
