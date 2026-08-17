{
  description = "A C++ parser for a minimal XML-like dialect";

  inputs = {
    nixpkgs.url = "github:NixOS/nixpkgs?ref=nixos-unstable";

    tinycmmc.url = "git+https://github.com/grumbel/tinycmmc.git";
    tinycmmc.inputs.nixpkgs.follows = "nixpkgs";
  };

  outputs = { self, nixpkgs, tinycmmc }:
    tinycmmc.lib.eachSystemWithPkgs (pkgs:
      {
        packages = rec {
          default = babyxml;

          babyxml = pkgs.stdenv.mkDerivation {
            pname = "babyxml";
            version = "0.0.0";

            src = ./.;

            cmakeFlags = [
              "-DBUILD_EXTRA=ON"
            ];

            nativeBuildInputs = with pkgs; [
              buildPackages.cmake
            ];

            buildInputs = [
              tinycmmc.packages.${pkgs.stdenv.hostPlatform.system}.default
            ];
           };
        };
      }
    );
}
