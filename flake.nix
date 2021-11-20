rec {
  description = "A classic 2d jump'n shoot game";

  inputs = {
    nixpkgs.url = "github:grumbel/nixpkgs/fix-guile-3.0";
    # nixpkgs.url = "github:nixos/nixpkgs";
    nix.inputs.nixpkgs.follows = "nixpkgs";
    flake-utils.url = "github:numtide/flake-utils";

    tinycmmc.url = "gitlab:grumbel/cmake-modules";
    tinycmmc.inputs.flake-utils.follows = "flake-utils";
    tinycmmc.inputs.nix.follows = "nix";
    tinycmmc.inputs.nixpkgs.follows = "nixpkgs";

    clanlib.url = "gitlab:grumbel/clanlib-1.0";
    clanlib.inputs.nix.follows = "nix";
    clanlib.inputs.nixpkgs.follows = "nixpkgs";
    clanlib.inputs.flake-utils.follows = "flake-utils";
  };

  outputs = { self, nix, nixpkgs, flake-utils, tinycmmc, clanlib }:
    flake-utils.lib.eachDefaultSystem (system:
      let
        pkgs = nixpkgs.legacyPackages.${system};
      in rec {
        packages = flake-utils.lib.flattenTree {
          windstille-0_2 = pkgs.stdenv.mkDerivation {
            pname = "windstille-0.2";
            version = "0.3.0";
            src = nixpkgs.lib.cleanSource ./.;
            cmakeFlags = [ "-DBUILD_EXTRA=ON" ];
            nativeBuildInputs = [
              pkgs.cmake
              pkgs.ninja
              pkgs.gcc
              pkgs.pkgconfig
              pkgs.makeWrapper
              tinycmmc.defaultPackage.${system}
            ];
            postFixup = ''
                wrapProgram $out/bin/windstille-0.2 \
                  --prefix LIBGL_DRIVERS_PATH ":" "${pkgs.mesa.drivers}/lib/dri" \
                  --prefix LD_LIBRARY_PATH ":" "${pkgs.mesa.drivers}/lib"
            '';
            buildInputs = [
              clanlib.defaultPackage.${system}

              pkgs.guile_3_0
              pkgs.libGL
              pkgs.libGLU
              pkgs.swig
              pkgs.x11
            ];
           };
        };
        defaultPackage = packages.windstille-0_2;
      });
}
