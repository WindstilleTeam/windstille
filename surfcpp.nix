{ stdenv
, lib
, SDL2
, cmake
, gbenchmark
, geomcpp
, gtest
, imagemagick6
, libexif
, libjpeg
, libpng
, logmich
, pkg-config
, tinycmmc
, withImagemagick ? false
, withLibexif ? false
, withSDL2 ? false
}:

stdenv.mkDerivation {
  pname = "surfcpp";
  version = "0.3.0";
  meta = {
    mainProgram = "surftool";
  };
  src = lib.cleanSource ./.;
  cmakeFlags = [
    "-DBUILD_EXTRA=ON"
  ]
  ++ lib.optional (!stdenv.hostPlatform.isWindows) "-DBUILD_BENCHMARK=ON"
  ++ (lib.optional withImagemagick "-DWITH_MAGICKXX=ON")
  ++ (lib.optional withLibexif "-DWITH_EXIF=ON")
  ++ (lib.optional withSDL2 "-DWITH_SDL2=ON");
  nativeBuildInputs = [
    cmake
    pkg-config
  ];
  buildInputs = [
    tinycmmc
    gtest
    SDL2
  ] ++ lib.optional (!stdenv.hostPlatform.isWindows) gbenchmark;
  propagatedBuildInputs = [
    geomcpp
    logmich
    libpng
    libjpeg
  ]
  ++ (lib.optional withImagemagick imagemagick6)
  ++ (lib.optional withLibexif libexif);
}
