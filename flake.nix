{
  description = "Windstille Input Engine";

  inputs = {
    nixpkgs.url = "github:NixOS/nixpkgs?ref=nixos-unstable";

    tinycmmc.url = "git+https://github.com/grumbel/tinycmmc.git";
    tinycmmc.inputs.nixpkgs.follows = "nixpkgs";
    tinycmmc.inputs.flake-utils.follows = "flake-utils";

    logmich.url = "git+https://github.com/logmich/logmich.git";
    logmich.inputs.nixpkgs.follows = "nixpkgs";
    logmich.inputs.tinycmmc.follows = "tinycmmc";

    priocpp.url = "git+https://github.com/grumbel/priocpp.git";
    priocpp.inputs.nixpkgs.follows = "nixpkgs";
    priocpp.inputs.flake-utils.follows = "flake-utils";
    priocpp.inputs.tinycmmc.follows = "tinycmmc";
    priocpp.inputs.logmich.follows = "logmich";
    priocpp.inputs.sexpcpp.follows = "sexpcpp";

    sexpcpp.url = "git+https://github.com/lispparser/sexp-cpp.git";
    sexpcpp.inputs.nixpkgs.follows = "nixpkgs";
    sexpcpp.inputs.flake-utils.follows = "flake-utils";
    sexpcpp.inputs.tinycmmc.follows = "tinycmmc";

    SDL2-win32.url = "git+https://github.com/grumnix/SDL2-win32.git";
    SDL2-win32.inputs.nixpkgs.follows = "nixpkgs";
    SDL2-win32.inputs.tinycmmc.follows = "tinycmmc";
  };

  outputs = { self, nixpkgs, flake-utils, tinycmmc, logmich, priocpp, sexpcpp, SDL2-win32 }:

    tinycmmc.lib.eachSystemWithPkgs (pkgs:
      {
        packages = rec {
          default = wstinput;

          wstinput = pkgs.stdenv.mkDerivation {
            pname = "wstinput";
            version = "0.3.0";

            src = nixpkgs.lib.cleanSource ./.;

            nativeBuildInputs = [
              tinycmmc.packages.${pkgs.stdenv.hostPlatform.system}.default

              pkgs.buildPackages.cmake
              pkgs.buildPackages.pkg-config
            ];

            propagatedBuildInputs = [
              logmich.packages.${pkgs.stdenv.hostPlatform.system}.default
              priocpp.packages.${pkgs.stdenv.hostPlatform.system}.default
              sexpcpp.packages.${pkgs.stdenv.hostPlatform.system}.default

              (if pkgs.stdenv.hostPlatform.isWindows
               then SDL2-win32.packages.${pkgs.stdenv.hostPlatform.system}.default
               else pkgs.SDL2)
            ];
           };
        };
      }
    );
}
