{
  description = "A classic 2d jump'n shoot game";

  inputs = {
    nixpkgs.url = "github:grumbel/nixpkgs/fix-guile-3.0";
    # nixpkgs.url = "github:NixOS/nixpkgs/nixos-22.05";
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
            src = nixpkgs.lib.cleanSource ./.;
            cmakeFlags = [ "-DBUILD_EXTRA=ON" ];
            postPatch = ''
              for file in test/*.?xx; do \
                substituteInPlace "$file" \
                  --replace '"data/' "\"$out/share/${pname}/"; \
              done
            '';
            postFixup = ''
              for program in $out/bin/*; do \
                wrapProgram $program \
                  --prefix LIBGL_DRIVERS_PATH ":" "${pkgs.mesa.drivers}/lib/dri" \
                  --prefix LD_LIBRARY_PATH ":" "${pkgs.mesa.drivers}/lib"
              done
            '';
            nativeBuildInputs = [
              pkgs.cmake
              pkgs.ninja
              pkgs.gcc
              pkgs.pkgconfig
              pkgs.makeWrapper
              tinycmmc.packages.${system}.default
            ];
            buildInputs = [
              clanlib.packages.${system}.default

              pkgs.guile_3_0
              pkgs.libGL
              pkgs.libGLU
              pkgs.swig
              pkgs.x11
            ];
           };
        };
      }
    );
}
