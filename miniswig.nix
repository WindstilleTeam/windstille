{ stdenv
, lib
, cmake
, pkg-config
, flex
, bison
, squirrel
, version
}:

stdenv.mkDerivation rec {
  pname = "miniswig";
  inherit version;

  src = ./.;

  # FIXME: miniswig.exe wants .dlls but can't find them
  doCheck = ! stdenv.hostPlatform.isWindows;

  cmakeFlags = [
    "-DPROJECT_VERSION_FULL=${version}"
  ] ++ lib.optional doCheck "-DBUILD_TESTS=ON";

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
