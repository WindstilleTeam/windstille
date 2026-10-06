# Windows x86_64 cross build (MinGW-w64) of the game.
#
# SDL2, OpenAL Soft and libmodplug come prebuilt from the grumnix *-win32
# flakes, squirrel is cross-built as a static library and the vendored
# libraries in external/ are built as part of the game (subdirectory
# fallback, like the other ports).
{ pkgs
, win64Pkgs
, src
, version
, miniswig  # host build, generates the squirrel wrapper
, squirrelSrc
, stbImageIncludeDir
, sdl2
, openal
, modplug
}:

let
  squirrelWin64 = win64Pkgs.stdenv.mkDerivation {
    pname = "squirrel-win64";
    version = "3.2";
    src = squirrelSrc;
    nativeBuildInputs = [ pkgs.cmake ];
    cmakeFlags = [
      "-DDISABLE_STATIC=OFF"
      "-DDISABLE_DYNAMIC=ON"
      "-DCMAKE_POLICY_VERSION_MINIMUM=3.5"
    ];
    # Upstream names the archives *_static.a and may use lib64/
    postInstall = ''
      mkdir -p "$out/lib"
      for name in squirrel sqstdlib; do
        f=$(find "$out" -name "lib''${name}_static.a" -o -name "lib''${name}.a" | head -1)
        if [ -z "$f" ]; then
          echo "error: lib$name not installed" >&2
          exit 1
        fi
        [ "$f" = "$out/lib/lib$name.a" ] || mv "$f" "$out/lib/lib$name.a"
      done
    '';
  };

  # Runtime DLLs that ship next to the .exe
  runtimeDllDirs = [
    "${sdl2}/bin"
    "${openal}/bin"
    "${modplug}/bin"
    "${win64Pkgs.freetype}/bin"
    "${win64Pkgs.zlib}/bin"
    "${win64Pkgs.bzip2}/bin"
    "${win64Pkgs.libpng}/bin"
    "${win64Pkgs.brotli.lib}/bin"
    "${win64Pkgs.windows.mcfgthreads}/bin"
    "${win64Pkgs.stdenv.cc.cc}/x86_64-w64-mingw32/lib"
    "${win64Pkgs.stdenv.cc.cc.lib}/x86_64-w64-mingw32/lib"
  ];

  game = win64Pkgs.stdenv.mkDerivation {
    pname = "windstille";
    inherit version src;

    nativeBuildInputs = [
      pkgs.cmake
      pkgs.pkg-config
    ];
    buildInputs = [
      sdl2
      openal
      modplug
      win64Pkgs.freetype
      win64Pkgs.zlib
    ];

    cmakeFlags = [
      "-DBUILD_EDITOR=OFF"
      "-DBUILD_EXTRA=OFF"
      "-DBUILD_TESTS=OFF"
      "-DPRIO_USE_JSONCPP=OFF"
      "-DPROJECT_VERSION_FULL=${version}"
      "-DMINISWIG=${miniswig}/bin/miniswig"
      # Upstream glm headers, nixpkgs patches gtc/packing.inl to include
      # <endian.h>, which MinGW lacks
      "-DWINDSTILLE_GLM_INCLUDE_DIR=${pkgs.glm.src}"
      "-DSQUIRREL_LIBRARIES=${squirrelWin64}/lib/libsquirrel.a;${squirrelWin64}/lib/libsqstdlib.a"
      "-DSQUIRREL_INCLUDE_DIRS=${squirrelWin64}/include"
      "-DWITH_STB=ON"
      "-DSTB_INCLUDE_DIR=${stbImageIncludeDir}"
      # Same slim codec set as the other ports: WAV and modplug modules
      "-DWSTSOUND_WITH_VORBIS=OFF"
      "-DWSTSOUND_WITH_OPUS=OFF"
      "-DWSTSOUND_WITH_MPG123=OFF"
    ];

    meta = {
      description = "Windstille game binary for Windows x86_64 (MinGW-w64)";
      platforms = [ "x86_64-windows" ];
    };
  };

in
{
  inherit squirrelWin64 game;

  # Flat, zip-friendly layout: windstille.exe, the DLLs and data/
  package = pkgs.runCommand "windstille-win64-${version}" {
    nativeBuildInputs = [ win64Pkgs.buildPackages.binutils ];
    meta = {
      description = "Windstille for Windows x86_64 (exe, DLLs and data)";
      platforms = pkgs.lib.platforms.linux;
    };
  } ''
    mkdir -p $out
    cp -v ${game}/bin/*.exe $out/
    cp -a ${src}/data $out/data

    # Copy the DLLs the .exe needs, following their dependencies
    dll_deps() {
      x86_64-w64-mingw32-objdump -p "$1" | sed -n 's/^\s*DLL Name: //p'
    }
    find_dll() {
      for dir in ${pkgs.lib.concatStringsSep " " runtimeDllDirs}; do
        if [ -f "$dir/$1" ]; then echo "$dir/$1"; return; fi
      done
    }
    queue=$(ls $out/*.exe)
    while [ -n "$queue" ]; do
      next=""
      for f in $queue; do
        for dll in $(dll_deps "$f"); do
          [ -e "$out/$dll" ] && continue
          path=$(find_dll "$dll")
          # system DLLs (KERNEL32.dll, opengl32.dll, …) are not shipped
          [ -z "$path" ] && continue
          cp -vL "$path" "$out/$dll"
          chmod u+w "$out/$dll"
          next="$next $out/$dll"
        done
      done
      queue="$next"
    done
  '';
}
