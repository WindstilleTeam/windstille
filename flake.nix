{
  description = "Windstille GUI Engine";

  inputs = {
    nixpkgs.url = "github:NixOS/nixpkgs?ref=nixos-unstable";
    flake-utils.url = "github:numtide/flake-utils";

    # Still required as a remote input because squirrel depends on it
    # (via follows) and is not available as a subtree.
    tinycmmc.url = "git+https://github.com/grumbel/tinycmmc.git";
    tinycmmc.inputs.nixpkgs.follows = "nixpkgs";
    tinycmmc.inputs.flake-utils.follows = "flake-utils";

    squirrel.url = "git+https://github.com/grumnix/squirrel.git";
    squirrel.inputs.nixpkgs.follows = "nixpkgs";
    squirrel.inputs.tinycmmc.follows = "tinycmmc";

    SDL2-win32.url = "git+https://github.com/grumnix/SDL2-win32.git";
    SDL2-win32.inputs.nixpkgs.follows = "nixpkgs";
  };

  outputs = { self, nixpkgs, flake-utils, tinycmmc, squirrel, SDL2-win32 }:
    let
      # Prefer the local tinycmmc subtree for the library helpers used
      # by this flake; the remote input is only kept to satisfy
      # squirrel's dependency.
      tinycmmc_lib = import ./external/tinycmmc { inherit nixpkgs flake-utils; };
    in
    tinycmmc_lib.eachSystemWithPkgs (pkgs:
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
            ] ++
            (nixpkgs.lib.optional pkgs.stdenv.hostPlatform.isLinux pkgs.makeWrapper);

            buildInputs = [
              # The former flake-provided libraries (argpp, babyxml, ...,
              # wstgui, ...) are now built from the external/ subtrees by
              # tinycmmc_find_dependency().  Only their transitive
              # dependencies that come from nixpkgs (or the remaining
              # remote flakes) need to be listed here.
              pkgs.fmt
              pkgs.gtest
              pkgs.glm
              pkgs.jsoncpp
              pkgs.libjpeg
              pkgs.libpng
              pkgs.glew
              pkgs.libGL
              pkgs.SDL2
              pkgs."sigc++"
              pkgs.sysprof

              squirrel.packages.${pkgs.stdenv.hostPlatform.system}.default
            ] ++
            (nixpkgs.lib.optionals pkgs.stdenv.hostPlatform.isLinux [
              pkgs.gtkmm3
            ]) ++
            (nixpkgs.lib.optionals pkgs.stdenv.hostPlatform.isWindows [
              SDL2-win32.packages.${pkgs.stdenv.hostPlatform.system}.default
            ]);
          };
        };
      }
    );
}
