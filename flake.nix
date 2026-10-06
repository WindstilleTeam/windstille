{
  description = "Binary file I/O";

  inputs = {
    nixpkgs.url = "github:NixOS/nixpkgs?ref=nixos-unstable";
    flake-utils.url = "github:numtide/flake-utils";
  };

  outputs = { self, nixpkgs, flake-utils }:
    let
      versionBase = nixpkgs.lib.strings.removeSuffix "\n" (builtins.readFile ./VERSION);
      gitRev = "${self.shortRev or self.dirtyShortRev or "dirty"}";
      isDev = nixpkgs.lib.strings.hasInfix "-dev" versionBase;
      version =
        if isDev then
          "${versionBase}.${toString (self.revCount or 0)}+g${gitRev}"
        else
          versionBase;
    in
    flake-utils.lib.eachDefaultSystem (system:
      let
        pkgs = nixpkgs.legacyPackages.${system};
      in
      {
        packages = rec {
          default = biiocpp;

          biiocpp = pkgs.stdenv.mkDerivation {
            pname = "biiocpp";
            inherit version;

            src = nixpkgs.lib.cleanSource ./.;

            cmakeFlags = [
              "-DPROJECT_VERSION_FULL=${version}"
            ];

            nativeBuildInputs = [
              pkgs.buildPackages.cmake
            ];

            buildInputs = [
              pkgs.gtest
            ];
          };
        };
      }
    );
}
