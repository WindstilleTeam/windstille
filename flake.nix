rec {
  description = "Windstille GUI Engine";

  inputs = {
    nixpkgs.url = "github:NixOS/nixpkgs/nixos-21.11";
    flake-utils.url = "github:numtide/flake-utils";

    tinycmmc.url = "gitlab:grumbel/cmake-modules";
    tinycmmc.inputs.nixpkgs.follows = "nixpkgs";
    tinycmmc.inputs.flake-utils.follows = "flake-utils";

    argparser.url = "gitlab:argparser/argparser/stable";
    argparser.inputs.nixpkgs.follows = "nixpkgs";
    argparser.inputs.flake-utils.follows = "flake-utils";
    argparser.inputs.tinycmmc.follows = "tinycmmc";

    logmich.url = "gitlab:logmich/logmich";
    logmich.inputs.nixpkgs.follows = "nixpkgs";
    logmich.inputs.flake-utils.follows = "flake-utils";
    logmich.inputs.tinycmmc.follows = "tinycmmc";

    geomcpp.url = "gitlab:grumbel/geomcpp";
    geomcpp.inputs.nixpkgs.follows = "nixpkgs";
    geomcpp.inputs.flake-utils.follows = "flake-utils";
    geomcpp.inputs.tinycmmc.follows = "tinycmmc";

    priocpp.url = "gitlab:grumbel/priocpp";
    priocpp.inputs.nixpkgs.follows = "nixpkgs";
    priocpp.inputs.flake-utils.follows = "flake-utils";
    priocpp.inputs.tinycmmc.follows = "tinycmmc";
    priocpp.inputs.logmich.follows = "logmich";
    priocpp.inputs.sexpcpp.follows = "sexpcpp";

    surfcpp.url = "gitlab:grumbel/surfcpp";
    surfcpp.inputs.nixpkgs.follows = "nixpkgs";
    surfcpp.inputs.flake-utils.follows = "flake-utils";
    surfcpp.inputs.tinycmmc.follows = "tinycmmc";
    surfcpp.inputs.geomcpp.follows = "geomcpp";
    surfcpp.inputs.logmich.follows = "logmich";

    babyxml.url = "gitlab:grumbel/babyxml";
    babyxml.inputs.nixpkgs.follows = "nixpkgs";
    babyxml.inputs.flake-utils.follows = "flake-utils";
    babyxml.inputs.tinycmmc.follows = "tinycmmc";

    sexpcpp.url = "gitlab:lispparser/sexp-cpp";
    sexpcpp.inputs.nixpkgs.follows = "nixpkgs";
    sexpcpp.inputs.flake-utils.follows = "flake-utils";
    sexpcpp.inputs.tinycmmc.follows = "tinycmmc";

    biiocpp.url = "gitlab:grumbel/biiocpp";
    biiocpp.inputs.nixpkgs.follows = "nixpkgs";
    biiocpp.inputs.flake-utils.follows = "flake-utils";
    biiocpp.inputs.tinycmmc.follows = "tinycmmc";

    wstinput.url = "gitlab:windstille/wstinput";
    wstinput.inputs.nixpkgs.follows = "nixpkgs";
    wstinput.inputs.flake-utils.follows = "flake-utils";
    wstinput.inputs.tinycmmc.follows = "tinycmmc";
    wstinput.inputs.logmich.follows = "logmich";
    wstinput.inputs.priocpp.follows = "priocpp";
    wstinput.inputs.sexpcpp.follows = "sexpcpp";

    wstdisplay.url = "gitlab:windstille/wstdisplay";
    wstdisplay.inputs.nixpkgs.follows = "nixpkgs";
    wstdisplay.inputs.flake-utils.follows = "flake-utils";
    wstdisplay.inputs.tinycmmc.follows = "tinycmmc";
    wstdisplay.inputs.geomcpp.follows = "geomcpp";
    wstdisplay.inputs.babyxml.follows = "babyxml";
    wstdisplay.inputs.surfcpp.follows = "surfcpp";
    wstdisplay.inputs.logmich.follows = "logmich";

    wstgui.url = "gitlab:windstille/wstgui";
    wstgui.inputs.nixpkgs.follows = "nixpkgs";
    wstgui.inputs.flake-utils.follows = "flake-utils";
    wstgui.inputs.tinycmmc.follows = "tinycmmc";
    wstgui.inputs.babyxml.follows = "babyxml";
    wstgui.inputs.geomcpp.follows = "geomcpp";
    wstgui.inputs.logmich.follows = "logmich";
    wstgui.inputs.priocpp.follows = "priocpp";
    wstgui.inputs.sexpcpp.follows = "sexpcpp";
    wstgui.inputs.surfcpp.follows = "surfcpp";
    wstgui.inputs.wstinput.follows = "wstinput";
    wstgui.inputs.wstdisplay.follows = "wstdisplay";
    wstgui.inputs.wstsound.follows = "wstsound";

    wstsound.url = "gitlab:windstille/wstsound";
    wstsound.inputs.nixpkgs.follows = "nixpkgs";
    wstsound.inputs.flake-utils.follows = "flake-utils";
    wstsound.inputs.tinycmmc.follows = "tinycmmc";

    miniswig.url = "gitlab:windstille/miniswig";
    miniswig.inputs.nixpkgs.follows = "nixpkgs";
    miniswig.inputs.flake-utils.follows = "flake-utils";
  };

  outputs = { self, nixpkgs, flake-utils,
              tinycmmc, argparser, logmich, geomcpp, priocpp, surfcpp, babyxml, sexpcpp, biiocpp,
              wstinput, wstdisplay, wstgui, wstsound, miniswig }:
    flake-utils.lib.eachDefaultSystem (system:
      let
        pkgs = nixpkgs.legacyPackages.${system};
      in rec {
        packages = flake-utils.lib.flattenTree rec {
          squirrel = pkgs.stdenv.mkDerivation {
            pname = "squirrel";
            version = "3.2";
            src = pkgs.fetchgit {
              url = "https://github.com/albertodemichelis/squirrel.git";
              #rev = "dda695e95f221b5e038796eab1f13d734dbd7270";
              #hash = "sha256-6gT/KT6o+a2pOA8u5r/qfaFAvet4rvR7nxynhXiv46w=";
              rev = "v3.2";
              hash = "sha256-vzAF0ooYoghw0yKKoS0Q6RnPPMhmP+05RoutVSZIGwk=";
            };
            nativeBuildInputs = [
              pkgs.cmake
            ];
          };

          windstille = pkgs.stdenv.mkDerivation {
            pname = "windstille";
            version = "0.3.0";
            src = nixpkgs.lib.cleanSource ./.;
            cmakeFlags = [ "-DBUILD_EXTRA=ON" ];
            nativeBuildInputs = [
              pkgs.cmake
              pkgs.gcc
              pkgs.pkgconfig
              pkgs.makeWrapper
            ];
            postFixup = ''
                wrapProgram $out/bin/windstille \
                  --prefix LIBGL_DRIVERS_PATH ":" "${pkgs.mesa.drivers}/lib/dri" \
                  --prefix LD_LIBRARY_PATH ":" "${pkgs.mesa.drivers}/lib"
            '';
            buildInputs = [
              miniswig.defaultPackage.${system}
              tinycmmc.defaultPackage.${system}
              argparser.defaultPackage.${system}
              logmich.defaultPackage.${system}
              geomcpp.defaultPackage.${system}
              priocpp.defaultPackage.${system}
              surfcpp.defaultPackage.${system}
              babyxml.defaultPackage.${system}
              sexpcpp.defaultPackage.${system}
              biiocpp.defaultPackage.${system}
              wstgui.defaultPackage.${system}
              wstinput.defaultPackage.${system}
              wstsound.defaultPackage.${system}
              wstdisplay.defaultPackage.${system}
              squirrel

              pkgs.freetype
              pkgs.libGL
              pkgs.libGLU
              pkgs.glew
              pkgs.gtest
              pkgs.xcftools
              pkgs.bison
              pkgs.flex
              miniswig
              pkgs.gtkmm3

              pkgs.fmt
              pkgs.glm
              pkgs.SDL2
              pkgs.libjpeg
              pkgs.libpng
              pkgs.libsigcxx
              pkgs.imagemagick6
              pkgs.libexif

              pkgs.openal
              pkgs.libvorbis
              pkgs.libogg
              pkgs.opusfile
              pkgs.mpg123
              pkgs.libmodplug
              pkgs.gtest

              pkgs.jsoncpp
            ];
           };
        };
        defaultPackage = packages.windstille;
      });
}
