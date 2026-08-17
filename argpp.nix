{ stdenv
, lib
, cmake
, fmt
, gtest
, strutcpp
, tinycmmc
}:

stdenv.mkDerivation {
  pname = "argpp";
  version = "2.0.0-beta";
  src = lib.cleanSource ./.;
  cmakeFlags = [
    "-DBUILD_TESTS=ON"
    "-DWARNINGS=ON"
    "-DWERROR=ON"
  ];
  nativeBuildInputs = [
    cmake
  ];
  buildInputs = [
    fmt
    gtest
    strutcpp
    tinycmmc
  ];
}
