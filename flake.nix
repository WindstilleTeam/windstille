{
  description = "A classic 2d jump'n shoot game";

  inputs = {
    nixpkgs.url = "github:NixOS/nixpkgs?ref=nixos-24.05";
    flake-utils.url = "github:numtide/flake-utils";

    tinycmmc.url = "github:grumbel/tinycmmc";
    tinycmmc.inputs.flake-utils.follows = "flake-utils";
    tinycmmc.inputs.nixpkgs.follows = "nixpkgs";

    clanlib.url = "github:grumbel/clanlib-1.0";
    clanlib.inputs.nixpkgs.follows = "nixpkgs";
    clanlib.inputs.flake-utils.follows = "flake-utils";
  };

  outputs = { self, nixpkgs, flake-utils, tinycmmc, clanlib }:
    flake-utils.lib.eachDefaultSystem (system:
      let
        pkgs = nixpkgs.legacyPackages.${system};
      in {
        packages = rec {
          default = windstille-0_2;

          windstille-0_2 = pkgs.stdenv.mkDerivation rec {
            pname = "windstille-0.2";
            version = "0.3.0";

            src = ./.;

            cmakeFlags = [
              "-DBUILD_EXTRA=ON"
            ];

            postPatch = ''
              for file in test/*.?xx; do \
                substituteInPlace "$file" \
                  --replace '"data/' "\"$out/share/${pname}/"; \
              done
            '';

            nativeBuildInputs = with pkgs; [
              cmake
              pkg-config
            ] ++ [
              tinycmmc.packages.${system}.default
            ];

            buildInputs = with pkgs; [
              guile_3_0
              libGL
              libGLU
              swig
              xorg.libX11
            ] ++ [
              clanlib.packages.${system}.default
            ];
           };
        };
      }
    );
}
