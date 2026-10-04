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
, withImagemagick ? false
, withLibexif ? false
, withSDL2 ? false
, version ? "0.0.0"
}:

stdenv.mkDerivation {
  pname = "surfcpp";
  inherit version;
  meta = {
    mainProgram = "surftool";
  };
  src = lib.cleanSource ./.;
  cmakeFlags = [
    "-DBUILD_EXTRA=ON"
    "-DPROJECT_VERSION_FULL=${version}"
  ]
  ++ lib.optional (!stdenv.hostPlatform.isWindows) "-DBUILD_BENCHMARKS=ON"
  ++ (lib.optional withImagemagick "-DWITH_MAGICKXX=ON")
  ++ (lib.optional withLibexif "-DWITH_EXIF=ON")
  ++ (lib.optional withSDL2 "-DWITH_SDL2=ON");
  nativeBuildInputs = [
    cmake
    pkg-config
  ];
  buildInputs = [
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
