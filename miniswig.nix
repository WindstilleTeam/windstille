{ stdenv
, lib
, cmake
, pkg-config
, flex
, bison
, squirrel
}:

stdenv.mkDerivation rec {
  pname = "miniswig";
  version = "0.0.0";

  src = ./.;

  # FIXME: miniswig.exe wants .dlls but can't find them
  doCheck = ! stdenv.targetPlatform.isWindows;

  cmakeFlags =
    lib.optional doCheck "-DBUILD_TESTS=ON";

  makeFlags = [
    "VERBOSE=1"
    "ARGS=-V"
  ];

  nativeBuildInputs = [
    cmake
    pkg-config
    flex
    bison
  ];

  buildInputs =
    lib.optionals doCheck checkInputs;

  checkInputs = [
    squirrel
  ];
}
