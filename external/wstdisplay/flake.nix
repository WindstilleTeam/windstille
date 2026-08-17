{
  description = "Windstille Display Engine";

  inputs = {
    nixpkgs.url = "github:NixOS/nixpkgs?ref=nixos-unstable";

    tinycmmc.url = "git+https://github.com/grumbel/tinycmmc.git";
    tinycmmc.inputs.nixpkgs.follows = "nixpkgs";

    babyxml.url = "git+https://github.com/grumbel/babyxml.git";
    babyxml.inputs.nixpkgs.follows = "nixpkgs";
    babyxml.inputs.tinycmmc.follows = "tinycmmc";

    logmich.url = "git+https://github.com/logmich/logmich.git";
    logmich.inputs.nixpkgs.follows = "nixpkgs";
    logmich.inputs.tinycmmc.follows = "tinycmmc";

    geomcpp.url = "git+https://github.com/grumbel/geomcpp.git";
    geomcpp.inputs.nixpkgs.follows = "nixpkgs";
    geomcpp.inputs.tinycmmc.follows = "tinycmmc";

    SDL2-win32.url = "git+https://github.com/grumnix/SDL2-win32.git";
    SDL2-win32.inputs.nixpkgs.follows = "nixpkgs";
    SDL2-win32.inputs.tinycmmc.follows = "tinycmmc";

    glew-win32.url = "git+https://github.com/grumnix/glew-win32.git";
    glew-win32.inputs.nixpkgs.follows = "nixpkgs";
    glew-win32.inputs.tinycmmc.follows = "tinycmmc";

    freetype-win32.url = "git+https://github.com/grumnix/freetype-win32.git";
    freetype-win32.inputs.nixpkgs.follows = "nixpkgs";
    freetype-win32.inputs.tinycmmc.follows = "tinycmmc";

    surfcpp.url = "git+https://github.com/grumbel/surfcpp.git";
    surfcpp.inputs.nixpkgs.follows = "nixpkgs";
    surfcpp.inputs.tinycmmc.follows = "tinycmmc";
    surfcpp.inputs.geomcpp.follows = "geomcpp";
    surfcpp.inputs.logmich.follows = "logmich";
    surfcpp.inputs.SDL2-win32.follows = "SDL2-win32";
  };

  outputs = { self, nixpkgs, tinycmmc, babyxml, geomcpp, logmich, surfcpp, freetype-win32, SDL2-win32, glew-win32 }:
    tinycmmc.lib.eachSystemWithPkgs (pkgs:
      rec {
        packages = rec {
          default = wstdisplay;

          wstdisplay = pkgs.stdenv.mkDerivation {
            pname = "wstdisplay";
            version = "0.3.0";

            src = nixpkgs.lib.cleanSource ./.;

            cmakeFlags = [ "-DBUILD_EXTRA=ON" ];

            postFixup =
              (nixpkgs.lib.optionalString pkgs.stdenv.hostPlatform.isWindows ''
                mkdir -p $out/bin/
                find ${pkgs.windows.mcfgthreads} -iname "*.dll" -exec ln -sfv {} $out/bin/ \;
                find ${pkgs.stdenv.cc.cc} -iname "*.dll" -exec ln -sfv {} $out/bin/ \;
                ln -sfv ${SDL2-win32.packages.${pkgs.stdenv.hostPlatform.system}.default}/bin/*.dll $out/bin/
                ln -sfv ${pkgs.fmt}/bin/*.dll $out/bin/
                ln -sfv ${pkgs.gtest}/bin/*.dll $out/bin/
                ln -sfv ${pkgs.libsigcxx}/bin/*.dll $out/bin/
                ln -sfv ${freetype-win32.packages.${pkgs.stdenv.hostPlatform.system}.default}/bin/*.dll $out/bin/
                ln -sfv ${glew-win32.packages.${pkgs.stdenv.hostPlatform.system}.default}/bin/*.dll $out/bin/

                # FIXME: These should be handled in surfcpp or statically linked
                ln -sfv ${pkgs.libjpeg_original.overrideAttrs (oldAttrs: { meta = {}; })}/bin/*.dll $out/bin/
                ln -sfv ${pkgs.libpng.overrideAttrs (oldAttrs: { meta = {}; })}/bin/*.dll $out/bin/
                ln -sfv ${pkgs.zlib.overrideAttrs (oldAttrs: { meta = {}; })}/bin/*.dll $out/bin/
            '');

            nativeBuildInputs = [
              pkgs.buildPackages.cmake
              pkgs.buildPackages.pkg-config
            ];

            buildInputs = [
              tinycmmc.packages.${pkgs.stdenv.hostPlatform.system}.default

              pkgs.gtest
            ];

            propagatedBuildInputs = [
              babyxml.packages.${pkgs.stdenv.hostPlatform.system}.default
              geomcpp.packages.${pkgs.stdenv.hostPlatform.system}.default
              logmich.packages.${pkgs.stdenv.hostPlatform.system}.default
              surfcpp.packages.${pkgs.stdenv.hostPlatform.system}.default

              pkgs.libsigcxx
              pkgs.fmt

              (if pkgs.stdenv.hostPlatform.isWindows
               then freetype-win32.packages.${pkgs.stdenv.hostPlatform.system}.default
               else pkgs.freetype)

              (if pkgs.stdenv.hostPlatform.isWindows
               then glew-win32.packages.${pkgs.stdenv.hostPlatform.system}.default
               else pkgs.glew)

              (if pkgs.stdenv.hostPlatform.isWindows
               then SDL2-win32.packages.${pkgs.stdenv.hostPlatform.system}.default
               else pkgs.SDL2)
            ];
          };
        };

        apps = rec {
          default = wstdisplay-geometry;

          wstdisplay-geometry = tinycmmc.inputs.flake-utils.lib.mkApp {
            drv = packages.wstdisplay;
            exePath = "/bin/wstdisplay-geometry";
          };
        };
      }
    );
}
