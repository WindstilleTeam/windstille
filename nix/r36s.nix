# R36S / ArkOS (RK3326) cross builds against a published ArkOS sysroot.
#
# The sysroot tarball supplies aarch64 headers + shared libs (glibc ~2.30 era,
# SDL2, GLES, …) so the binary can run on stock ArkOS — unlike pkgsCross
# alone, which links modern nixpkgs glibc/Mesa.
#
# Usage:
#   nix build .#arkos-sysroot
#   nix build .#windstille-r36s
#   nix build .#windstille-r36s-portmaster   # PortMaster tree for /roms/ports
#
{ lib
, stdenv
, stdenvNoCC
, fetchurl
, cmake
, pkg-config
, pkgsCross
, writeShellScript
, writeTextFile
, zip
, glm  # header-only; not present in ArkOS sysroot
, squirrelSrc ? null  # optional: path/tarball for upstream squirrel (cross-built static)
, bison ? null
, flex ? null
  # Attrset of label -> unwrapped aarch64 *cross* gcc (from pinned nixpkgs).
  # Native buildPackages.gccN must NOT be passed — they lack aarch64 c++config.
, extraCrossGccs ? {}
}:

let
  arkosSysrootSrc = fetchurl {
    name = "arkos-sysroot.tar.gz";
    # Placeholder URL for a published ArkOS aarch64 sysroot tarball
    # (glibc ~2.30 + SDL2 + GLES/EGL + OpenAL Soft + libmodplug).
    # Replace with a permanent host before CI/hydra; then:
    #   nix store prefetch-file <url>
    # and paste the new sha256-… into `hash`.
    url = "http://localhost:8888/arkos-sysroot4.tar.gz";
    hash = "sha256-Sm1Xcy++M6LuOLXs9nOs7xIfuvzqAOledKnWH8H7+/g=";
  };

  # Allow hash to be overridden by the user who already fetched the tarball;
  # if the placeholder remains, Nix will print the expected hash.
  khrplatformH = ../mk/r36s/include/KHR/khrplatform.h;

  # stb_image.h for PNG/JPEG when the ArkOS sysroot has no libpng/libjpeg
  # (same approach as the Android port).
  stbImageH = fetchurl {
    url = "https://raw.githubusercontent.com/nothings/stb/refs/heads/master/stb_image.h";
    sha256 = "sha256-WUwv411JSItDgtv67I+YNm3vyoGdkWrJW+zz519CALM=";
  };
  stbImageIncludeDir = stdenvNoCC.mkDerivation {
    name = "stb-image-include";
    dontUnpack = true;
    installPhase = ''
      mkdir -p $out
      cp ${stbImageH} $out/stb_image.h
    '';
  };

  arkosSysroot = stdenvNoCC.mkDerivation {
    pname = "arkos-sysroot";
    version = "0.1-openal";
    src = arkosSysrootSrc;

    # Unpack-only: aarch64 ELF + linker scripts must not be touched by the
    # host fixup (patchelf "wrong ELF type", strip, shebang rewrite).
    dontConfigure = true;
    dontBuild = true;
    dontFixup = true;
    dontPatchELF = true;
    dontStrip = true;
    dontPatchShebangs = true;
    dontCheckForBrokenSymlinks = true;

    installPhase = ''
      runHook preInstall
      mkdir -p "$out"

      if [ -d usr ]; then
        cp -a . "$out/"
      elif [ -d sysroot/usr ]; then
        cp -a sysroot/. "$out/"
      else
        top=
        for d in *; do
          if [ -d "$d/usr" ]; then top="$d"; break; fi
        done
        if [ -z "$top" ]; then
          echo "arkos-sysroot: unrecognized tarball layout (no usr/):" >&2
          find . -maxdepth 3 -type d >&2 || true
          exit 1
        fi
        cp -a "$top"/. "$out/"
      fi

      test -d "$out/usr" || {
        echo "arkos-sysroot: missing $out/usr after install" >&2
        exit 1
      }

      for base in "$out/usr/include" "$out/usr/lib" "$out/lib"; do
        if [ -d "$base/aarch64-linux-gnu" ] && [ ! -e "$base/aarch64-unknown-linux-gnu" ]; then
          ln -sfn aarch64-linux-gnu "$base/aarch64-unknown-linux-gnu"
        fi
      done

      mkdir -p "$out/usr/include/KHR"
      cp -f ${khrplatformH} "$out/usr/include/KHR/khrplatform.h"

      # Debian libc.so linker scripts embed absolute /lib/... paths. Rewrite
      # ONLY the multiarch absolute prefixes (not a bare "/lib/" which would
      # re-match inside /nix/store/.../lib/... and double the path).
      find "$out" -type f \( -name 'libc.so' -o -name 'libpthread.so' -o -name 'libm.so' -o -name 'libdl.so' -o -name 'librt.so' -o -name 'libutil.so' -o -name 'libresolv.so' -o -name 'libanl.so' -o -name 'libBrokenLocale.so' -o -name 'libthread_db.so' \) 2>/dev/null | while read -r f; do
        if grep -qE 'GROUP|INPUT' "$f" 2>/dev/null; then
          echo "patching linker script $f"
          # Match only when the path starts at a token boundary (space, (, =).
          sed -i -E \
            -e "s#(^|[[:space:](=])/usr/lib/aarch64-linux-gnu/#\1$out/usr/lib/aarch64-linux-gnu/#g" \
            -e "s#(^|[[:space:](=])/lib/aarch64-linux-gnu/#\1$out/lib/aarch64-linux-gnu/#g" \
            "$f" || true
        fi
      done

      ln -sfn . "$out/sysroot"
      echo "arkos-sysroot ready" > "$out/SYSROOT.txt"
      runHook postInstall
    '';

    meta = with lib; {
      description = "ArkOS / R36S aarch64 sysroot (glibc + SDL2 + GLES)";
      license = licenses.free;
      platforms = platforms.linux;
      hydraPlatforms = [];
    };
  };

  crossPkgs = pkgsCross.aarch64-multiplatform;
  crossCc = crossPkgs.stdenv.cc;
  targetPrefix = crossCc.targetPrefix;

  # Wrappers inject -nostdinc + ordered isystem so:
  #   1) libstdc++ *headers* from nixpkgs gcc (compile only)
  #   2) gcc fixed headers (stddef.h)
  #   3) ArkOS glibc headers only (never gcc's modern sys-include)
  # That avoids __attr_dealloc_free errors from mixing glibc 2.30 cdefs with
  # modern stdlib.h, and keeps #include_next <stdlib.h> working.
  #
  # Link is different: GCC 15's libstdc++/libgcc_s need GLIBC_2.32–2.38, but
  # ArkOS is ~2.30. g++ also injects an absolute path to its own libstdc++.so,
  # so -L order alone is ignored. We therefore:
  #   - compile with modern headers + _GLIBCXX_USE_CXX11_ABI=0 (old ABI)
  #   - -nostdlib++ so g++ does not force its libstdc++; link sysroot -lstdc++
  #   - -static-libgcc (libgcc.a / libgcc_eh.a have no versioned GLIBC_2.3x deps)
  #   - -fexceptions: Windstille/tinygettext/prio use try/catch (unlike SuperTux M1)
  #   - --allow-shlib-undefined for DT_NEEDED of sysroot libs (e.g. opusfile
  #     from SDL2_mixer) that exist on the device at runtime

  # Static squirrel for aarch64 (not in ArkOS sysroot). Cross-built so the
  # archives are ELF aarch64 and link into the game binary.
  squirrelSrcResolved =
    if squirrelSrc != null then squirrelSrc
    else fetchurl {
      url = "https://github.com/albertodemichelis/squirrel/archive/f77074bdd6152d230609146a3d424c6f49e3770f.tar.gz";
      hash = "sha256-hw4EFdN+KSwVR4spZkgVDeXor5H5w3KGvI6bslmSBW8=";
    };

  squirrelR36s = crossPkgs.stdenv.mkDerivation {
    pname = "squirrel-r36s";
    version = "3.2";
    src = squirrelSrcResolved;
    nativeBuildInputs = [ cmake ];
    dontUseCmakeConfigure = true;
    buildPhase = ''
      runHook preBuild
      srcdir="$PWD"
      if [ ! -f CMakeLists.txt ]; then
        srcdir="$(find . -maxdepth 2 -name CMakeLists.txt -printf '%h\n' | head -1)"
      fi
      mkdir -p build
      cmake -S "$srcdir" -B build \
        -DCMAKE_BUILD_TYPE=Release \
        -DCMAKE_INSTALL_PREFIX="$out" \
        -DDISABLE_STATIC=OFF \
        -DDISABLE_DYNAMIC=ON \
        -DCMAKE_POLICY_VERSION_MINIMUM=3.5
      cmake --build build -j''${NIX_BUILD_CORES:-2}
      cmake --install build || true
      mkdir -p "$out/lib" "$out/include"
      # Upstream installs libsquirrel_static.a into lib64/; normalize to lib/.
      for cand in libsquirrel_static.a libsquirrel.a squirrel.a; do
        f=$(find "$out" build . -name "$cand" 2>/dev/null | head -1)
        if [ -n "$f" ]; then cp -f "$f" "$out/lib/libsquirrel.a"; break; fi
      done
      for cand in libsqstdlib_static.a libsqstdlib.a sqstdlib.a; do
        f=$(find "$out" build . -name "$cand" 2>/dev/null | head -1)
        if [ -n "$f" ]; then cp -f "$f" "$out/lib/libsqstdlib.a"; break; fi
      done
      if [ ! -f "$out/lib/libsquirrel.a" ] || [ ! -f "$out/lib/libsqstdlib.a" ]; then
        echo "error: squirrel r36s static libs missing after build" >&2
        find . "$out" -name '*.a' 2>/dev/null | head -40 >&2 || true
        exit 1
      fi
      if [ -d "$srcdir/include" ]; then cp -a "$srcdir/include"/. "$out/include/"; fi
      if [ ! -f "$out/include/squirrel.h" ] && [ -f "$out/include/squirrel/squirrel.h" ]; then
        cp "$out/include/squirrel/squirrel.h" "$out/include/"
      fi
      runHook postBuild
    '';
    installPhase = "runHook preInstall; runHook postInstall";
  };


  freetypeR36s = crossPkgs.stdenv.mkDerivation rec {
    pname = "freetype-r36s";
    version = "2.13.2";
    src = fetchurl {
      url = "https://downloads.sourceforge.net/project/freetype/freetype2/${version}/freetype-${version}.tar.xz";
      hash = "sha256-EpkcTlXFBt1/m3ZZM+Yv0r4uBtQhUF15UKEy5PG7SE0=";
    };
    nativeBuildInputs = [ cmake ];
    cmakeFlags = [
      "-DCMAKE_BUILD_TYPE=Release"
      "-DBUILD_SHARED_LIBS=OFF"
      "-DFT_DISABLE_ZLIB=TRUE"
      "-DFT_DISABLE_BZIP2=TRUE"
      "-DFT_DISABLE_PNG=TRUE"
      "-DFT_DISABLE_HARFBUZZ=TRUE"
      "-DFT_DISABLE_BROTLI=TRUE"
      "-DCMAKE_POLICY_VERSION_MINIMUM=3.5"
    ];
    # CMake joins ${prefix}/@CMAKE_INSTALL_LIBDIR@ into broken //nix/store/… paths.
    postInstall = ''
      mkdir -p $out/lib/pkgconfig
      cat > $out/lib/pkgconfig/freetype2.pc <<EOF
prefix=$out
exec_prefix=''${prefix}
libdir=''${exec_prefix}/lib
includedir=''${prefix}/include/freetype2
Name: FreeType 2
Description: A free, high-quality, and portable font engine (static, R36S)
Version: ${version}
Libs: -L''${libdir} -lfreetype
Cflags: -I''${includedir}
EOF
    '';
  };

  mkWrappers = sysroot: let
    gcc = crossCc.cc;
    tp = lib.removeSuffix "-" targetPrefix; # aarch64-unknown-linux-gnu
    libdir = "${sysroot}/usr/lib/aarch64-linux-gnu";
    cxxInc = "${gcc}/include/c++/${gcc.version}";
    cxxIncTarget = "${cxxInc}/${tp}";
    fixedInc = "${gcc}/lib/gcc/${tp}/${gcc.version}/include";
    fixedInc2 = "${gcc}/lib/gcc/${tp}/${gcc.version}/include-fixed";
    libgccDir = "${gcc}/lib/gcc/${tp}/${gcc.version}";
    gccLibOut = lib.getLib gcc;
    libgccLib = "${gccLibOut}/lib";
    libgccLibTarget = "${gccLibOut}/${tp}/lib";
    # Compile-only flags (safe with -c). No -L/-B lib paths that pull Scrt1.o.
    # -fexceptions: tinygettext / prio / Windstille use C++ exceptions; pair with
    # -static-libgcc so libgcc_eh is not the shared GCC 15 copy (GLIBC_2.35).
    # Omit include-fixed: its pthread.h needs glibc ≥2.32; ArkOS is ~2.30.
    commonCompile = ''
      -nostdinc \
      --sysroot=${sysroot} \
      -isystem ${fixedInc} \
      -isystem ${sysroot}/usr/include/aarch64-linux-gnu \
      -isystem ${sysroot}/usr/include \
      -pthread \
      -fexceptions \
      -march=armv8-a \
      -mtune=cortex-a35 \
    '';
    commonCompileCxx = ''
      -nostdinc \
      -D_GLIBCXX_USE_CXX11_ABI=0 \
      --sysroot=${sysroot} \
      -isystem ${cxxInc} \
      -isystem ${cxxIncTarget} \
      -isystem ${cxxInc}/backward \
      -isystem ${fixedInc} \
      -isystem ${sysroot}/usr/include/aarch64-linux-gnu \
      -isystem ${sysroot}/usr/include \
      -pthread \
      -fexceptions \
      -DSEXP_NO_FLOAT_CHARCONV \
      -march=armv8-a \
      -mtune=cortex-a35 \
    '';
    # Link flags: sysroot first for libc/SDL; add modern gcc -L so
    # -static-libgcc can find libgcc.a / libgcc_eh.a (stdc++ is still the
    # absolute sysroot path in the cxx wrapper — not -lstdc++).
    # Explicit dynamic linker so the binary runs on ArkOS (not /nix/store/.../ld).
    commonLink = ''
      --sysroot=${sysroot} \
      -Wl,--sysroot=${sysroot} \
      -Wl,--dynamic-linker=/lib/ld-linux-aarch64.so.1 \
      -B${libdir} \
      -B${libgccDir} \
      -L${libdir} \
      -L${sysroot}/usr/lib \
      -L${sysroot}/lib \
      -L${sysroot}/lib/aarch64-linux-gnu \
      -L${libgccDir} \
      -L${libgccLib} \
      -L${libgccLibTarget} \
      -static-libgcc \
      -Wl,-Bdynamic \
      -l:libpthread.so.0 \
      -lm \
      -Wl,-rpath-link,${libdir} \
      -Wl,-rpath-link,${sysroot}/usr/lib/aarch64-linux-gnu \
      -Wl,-rpath-link,${sysroot}/lib/aarch64-linux-gnu \
      -Wl,-rpath-link,${sysroot}/usr/lib/aarch64-linux-gnu/pulseaudio \
      -Wl,-rpath-link,${sysroot}/lib/aarch64-linux-gnu/pulseaudio \
      -Wl,--allow-shlib-undefined \
      -Wl,--as-needed \
      -march=armv8-a \
      -mtune=cortex-a35 \
    '';
  in {
    cc = writeShellScript "aarch64-arkos-gcc" ''
      export PATH="${crossCc.bintools}/bin:$PATH"
      is_compile=
      for a in "$@"; do
        case "$a" in
          -c|-S|-E|-M|-MM|-MD|-MMD) is_compile=1 ;;
        esac
      done
      if [ -n "$is_compile" ]; then
        exec ${gcc}/bin/${targetPrefix}gcc \
          -B${crossCc.bintools}/bin \
          ${commonCompile} \
          "$@"
      else
        exec ${gcc}/bin/${targetPrefix}gcc \
          -B${crossCc.bintools}/bin \
          ${commonCompile} \
          ${commonLink} \
          "$@"
      fi
    '';
    # Link sysroot libstdc++ by absolute path so g++ cannot pick GCC 15's
    # (which requires GLIBCXX_3.4.32 not present on ArkOS).
    cxx = writeShellScript "aarch64-arkos-g++" ''
      export PATH="${crossCc.bintools}/bin:$PATH"
      is_compile=
      for a in "$@"; do
        case "$a" in
          -c|-S|-E|-M|-MM|-MD|-MMD) is_compile=1 ;;
        esac
      done
      if [ -n "$is_compile" ]; then
        exec ${gcc}/bin/${targetPrefix}g++ \
          -B${crossCc.bintools}/bin \
          ${commonCompileCxx} \
          "$@"
      else
        stdcpp=
        for cand in \
          "${libdir}/libstdc++.so" \
          "${libdir}/libstdc++.so.6" \
          "${sysroot}/usr/lib/libstdc++.so" \
          "${sysroot}/usr/lib/libstdc++.so.6" \
          "${sysroot}/lib/aarch64-linux-gnu/libstdc++.so" \
          "${sysroot}/lib/aarch64-linux-gnu/libstdc++.so.6"
        do
          if [ -e "$cand" ]; then stdcpp="$cand"; break; fi
        done
        if [ -z "$stdcpp" ]; then
          echo "aarch64-arkos-g++: no libstdc++ in sysroot" >&2
          exit 1
        fi
        sdl2=
        sdl2image=
        for cand in \
          "${libdir}/libSDL2-2.0.so" \
          "${libdir}/libSDL2-2.0.so.0" \
          "${libdir}/libSDL2.so" \
          "${sysroot}/usr/lib/libSDL2-2.0.so.0" \
          "${sysroot}/usr/lib/libSDL2.so"
        do
          if [ -e "$cand" ]; then sdl2="$cand"; break; fi
        done
        for cand in \
          "${libdir}/libSDL2_image-2.0.so" \
          "${libdir}/libSDL2_image-2.0.so.0" \
          "${libdir}/libSDL2_image.so" \
          "${sysroot}/usr/lib/libSDL2_image-2.0.so.0" \
          "${sysroot}/usr/lib/libSDL2_image.so"
        do
          if [ -e "$cand" ]; then sdl2image="$cand"; break; fi
        done
        if [ -z "$sdl2" ]; then
          echo "aarch64-arkos-g++: no libSDL2 in sysroot" >&2
          find "${sysroot}" -name 'libSDL2*' 2>/dev/null | head -20 >&2 || true
          exit 1
        fi
        if [ -z "$sdl2image" ]; then
          echo "aarch64-arkos-g++: no libSDL2_image in sysroot" >&2
          find "${sysroot}" -name 'libSDL2_image*' 2>/dev/null | head -20 >&2 || true
          exit 1
        fi
        extra_audio=
        for cand in \
          "${libdir}/libopenal.so" \
          "${libdir}/libopenal.so.1" \
          "${sysroot}/usr/lib/libopenal.so" \
          "${sysroot}/usr/lib/libopenal.so.1" \
          "${sysroot}/usr/lib/aarch64-linux-gnu/libopenal.so" \
          "${sysroot}/usr/lib/aarch64-linux-gnu/libopenal.so.1"
        do
          if [ -e "$cand" ]; then extra_audio="$extra_audio $cand"; break; fi
        done
        for cand in \
          "${libdir}/libmodplug.so" \
          "${libdir}/libmodplug.so.1" \
          "${sysroot}/usr/lib/libmodplug.so" \
          "${sysroot}/usr/lib/libmodplug.so.1" \
          "${sysroot}/usr/lib/aarch64-linux-gnu/libmodplug.so" \
          "${sysroot}/usr/lib/aarch64-linux-gnu/libmodplug.so.1"
        do
          if [ -e "$cand" ]; then extra_audio="$extra_audio $cand"; break; fi
        done
        exec ${gcc}/bin/${targetPrefix}g++ \
          -B${crossCc.bintools}/bin \
          ${commonCompileCxx} \
          -nostdlib++ \
          ${commonLink} \
          "$@" \
          -Wl,--no-as-needed "$stdcpp" "$sdl2image" "$sdl2" $extra_audio \
          -Wl,-Bdynamic -l:libpthread.so.0 -lm \
          -Wl,--as-needed
      fi
    '';
  };


  miniswigHost = stdenv.mkDerivation {
    pname = "miniswig-host";
    version = "0.1";
    src = ../external/miniswig;
    nativeBuildInputs = [ cmake ] ++ lib.optionals (bison != null) [ bison ]
      ++ lib.optionals (flex != null) [ flex ];
    cmakeFlags = [ "-DBUILD_TESTS=OFF" "-DCMAKE_POLICY_VERSION_MINIMUM=3.5" ];
  };

  mkWindstilleR36s = {
    src
  , version
  , pname ? "windstille-r36s"
  # Requires libopenal (+ optional libmodplug) in the published ArkOS sysroot.
  # PortMaster ships copies under windstille/libs/ so stock images without apt work.
  , enableSound ? true
  }:
    let
      wrappers = mkWrappers arkosSysroot;
    in
    stdenv.mkDerivation {
      inherit pname version src;

      nativeBuildInputs = [
        cmake
        pkg-config
        crossCc.bintools
        miniswigHost
      ];

      strictDeps = true;

      postPatch = ''
        mkdir -p src/system
        cp ${../mk/r36s/arkos_compat.c} src/system/arkos_compat.c
        cp ${../mk/r36s/arkos_compat.cpp} src/system/arkos_compat.cpp
      '';


      # Avoid host cmakeDefaults forcing the wrong compilers after our flags.
      cmakeFlags = [
        "-DCMAKE_SYSTEM_NAME=Linux"
        "-DCMAKE_SYSTEM_PROCESSOR=aarch64"
        "-DCMAKE_SYSROOT=${arkosSysroot}"
        "-DCMAKE_FIND_ROOT_PATH=${arkosSysroot}"
        "-DCMAKE_FIND_ROOT_PATH_MODE_PROGRAM=NEVER"
        "-DCMAKE_FIND_ROOT_PATH_MODE_LIBRARY=ONLY"
        "-DCMAKE_FIND_ROOT_PATH_MODE_INCLUDE=ONLY"
        "-DCMAKE_FIND_ROOT_PATH_MODE_PACKAGE=ONLY"
        "-DCMAKE_C_COMPILER=${wrappers.cc}"
        "-DCMAKE_CXX_COMPILER=${wrappers.cxx}"
        "-DCMAKE_C_COMPILER_WORKS=1"
        "-DCMAKE_CXX_COMPILER_WORKS=1"
        "-DCMAKE_C_COMPILER_FORCED=TRUE"
        "-DCMAKE_CXX_COMPILER_FORCED=TRUE"
        # Device binary must use ArkOS libs at runtime, not nix store paths.
        # Skipping RPATH rewrite also avoids cmake_install.cmake failing when
        # the linked RUNPATH does not contain the sysroot's /usr/lib/... path.
        "-DCMAKE_SKIP_RPATH=ON"
        "-DCMAKE_SKIP_INSTALL_RPATH=ON"
        "-DCMAKE_BUILD_WITH_INSTALL_RPATH=OFF"
        "-DCMAKE_INSTALL_RPATH="
        # RelWithDebInfo: keep -g for on-device gdb (exception unwind still
        # fragile on GCC 15 + ArkOS libgcc, but symbols make traces usable).
        "-DCMAKE_BUILD_TYPE=RelWithDebInfo"
        "-DBUILD_TESTS=OFF"
        "-DBUILD_EXTRA=OFF"
        "-DWARNINGS=OFF"
        "-DWERROR=OFF"
        # R36S is GLES2-only (Mali/Panfrost); desktop OpenGL is not in the sysroot.
        "-DWINDSTILLE_USE_GLES=ON"
        # No libpng/libjpeg in the published ArkOS sysroot — use stb_image
        # (same as Android) so PNG/JPEG assets actually load.
        "-DSURF_USE_STB_IMAGE=ON"
        "-DSTB_IMAGE_INCLUDE_DIR=${stbImageIncludeDir}"
        # ArkOS sysroot has neither libsigc++ nor glm cmake config.
        "-DWINDSTILLE_SIGC_POLYFILL_DIR=${../mk/android/app/jni}"
        "-DWINDSTILLE_GLM_INCLUDE_DIR=${glm}/include"
        "-DMINISWIG=${miniswigHost}/bin/miniswig"
        "-DBUILD_EDITOR=OFF"
        "-DWINDSTILLE_ARKOS_COMPAT_DIR=${../mk/r36s}"
        # Forced cross-compiler cannot try_compile pthread; ArkOS glibc has it.
        "-DCMAKE_HAVE_LIBC_PTHREAD=1"
        "-DCMAKE_THREAD_LIBS_INIT=-pthread"
        "-DPTHREAD_LIBRARY=pthread"
        "-DWINDSTILLE_ENABLE_SOUND=${if enableSound then "ON" else "OFF"}"
        # Slim codec set (same as Android/wasm): WAV + modplug modules only.
        "-DWSTSOUND_WITH_MPG123=OFF"
        "-DWSTSOUND_WITH_VORBIS=OFF"
        "-DWSTSOUND_WITH_OPUS=OFF"
        "-DWSTSOUND_WITH_MODPLUG=${if enableSound then "ON" else "OFF"}"
        "-DWSTSOUND_WITH_EFX=OFF"
        # No xdgcpp / jsoncpp in the published ArkOS sysroot (desktop helpers).
        "-DWINDSTILLE_NO_XDGCPP=ON"
        # GCC 15 headers vs ArkOS libstdc++: shim missing ABI symbols.
        "-DWINDSTILLE_CXXABI_SHIM=${../mk/r36s/cxxabi_shim.cpp}"
        # Relative data next to the binary on device (PortMaster layout).
        "-DPROJECT_VERSION_FULL=${version}"
        "-DSQUIRREL_LIBRARIES=${squirrelR36s}/lib/libsquirrel.a;${squirrelR36s}/lib/libsqstdlib.a"
        "-DSQUIRREL_INCLUDE_DIRS=${squirrelR36s}/include"
        "-DFREETYPE_LIBRARY=${freetypeR36s}/lib/libfreetype.a"
        "-DFREETYPE_INCLUDE_DIRS=${freetypeR36s}/include/freetype2"
        "-DFREETYPE_DIR=${freetypeR36s}"
      ];

      # Do not let nix stdenv rewrite RUNPATH to modern glibc / gcc-15 libs,
      # or shebangs on helper scripts to /nix/store/.../bash.
      dontPatchELF = true;
      dontStrip = true;
      dontPatchShebangs = true;

      preConfigure = ''
        # Prevent stdenv from injecting -rpath to modern nixpkgs glibc/gcc.
        export NIX_DONT_SET_RPATH=1
        export NIX_NO_SELF_RPATH=1

        export PKG_CONFIG="pkg-config"
        export PKG_CONFIG_SYSROOT_DIR="${arkosSysroot}"
        export PKG_CONFIG_DIR=""
        export PKG_CONFIG_PATH=""
        export PKG_CONFIG_LIBDIR="${arkosSysroot}/usr/lib/aarch64-linux-gnu/pkgconfig:${arkosSysroot}/usr/lib/pkgconfig:${arkosSysroot}/usr/share/pkgconfig"
        pkg-config --exists sdl2 && pkg-config --modversion sdl2 || true

        ZLIB_LIB=
        for cand in \
          "${arkosSysroot}/usr/lib/aarch64-linux-gnu/libz.so" \
          "${arkosSysroot}/lib/aarch64-linux-gnu/libz.so" \
          "${arkosSysroot}/usr/lib/aarch64-linux-gnu/libz.so.1" \
          "${arkosSysroot}/lib/aarch64-linux-gnu/libz.so.1"
        do
          if [ -e "$cand" ]; then ZLIB_LIB="$cand"; break; fi
        done
        if [ -z "$ZLIB_LIB" ]; then
          echo "arkos-sysroot: no libz.so found" >&2
          exit 1
        fi
        cmakeFlagsArray+=(
          "-DZLIB_INCLUDE_DIR=${arkosSysroot}/usr/include"
          "-DZLIB_LIBRARY=$ZLIB_LIB"
        )

        # Explicit GLES/EGL from ArkOS sysroot (find_library can miss under ONLY mode).
        GLESV2_LIB=
        EGL_LIB=
        for cand in           "${arkosSysroot}/usr/lib/aarch64-linux-gnu/libGLESv2.so"           "${arkosSysroot}/lib/aarch64-linux-gnu/libGLESv2.so"           "${arkosSysroot}/usr/lib/libGLESv2.so"
        do
          if [ -e "$cand" ]; then GLESV2_LIB="$cand"; break; fi
        done
        for cand in           "${arkosSysroot}/usr/lib/aarch64-linux-gnu/libEGL.so"           "${arkosSysroot}/lib/aarch64-linux-gnu/libEGL.so"           "${arkosSysroot}/usr/lib/libEGL.so"
        do
          if [ -e "$cand" ]; then EGL_LIB="$cand"; break; fi
        done
        if [ -z "$GLESV2_LIB" ]; then
          echo "arkos-sysroot: no libGLESv2.so found" >&2
          find "${arkosSysroot}" -name 'libGLESv2*' 2>/dev/null | head -20 >&2 || true
          exit 1
        fi
        cmakeFlagsArray+=(
          "-DWINDSTILLE_GLESV2_LIB=$GLESV2_LIB"
        )
        if [ -n "$EGL_LIB" ]; then
          cmakeFlagsArray+=(
            "-DWINDSTILLE_EGL_LIB=$EGL_LIB"
          )
        fi

        if [ "${if enableSound then "1" else "0"}" = "1" ]; then
          OPENAL_LIB=
          OPENAL_INC=
          for cand in \
            "${arkosSysroot}/usr/lib/aarch64-linux-gnu/libopenal.so" \
            "${arkosSysroot}/usr/lib/libopenal.so" \
            "${arkosSysroot}/lib/aarch64-linux-gnu/libopenal.so" \
            "${arkosSysroot}/usr/lib/aarch64-linux-gnu/libopenal.so.1" \
            "${arkosSysroot}/usr/lib/libopenal.so.1"
          do
            if [ -e "$cand" ]; then OPENAL_LIB="$cand"; break; fi
          done
          # Non-Emscripten wstsound includes <al.h>/<alext.h> (not <AL/al.h>),
          # so OPENAL_INCLUDE_DIR must be the AL/ directory itself.
          for cand in \
            "${arkosSysroot}/usr/include/AL" \
            "${arkosSysroot}/usr/include/aarch64-linux-gnu/AL"
          do
            if [ -f "$cand/al.h" ]; then OPENAL_INC="$cand"; break; fi
          done
          if [ -z "$OPENAL_LIB" ] || [ -z "$OPENAL_INC" ]; then
            echo "arkos-sysroot: OpenAL required for enableSound but not found" >&2
            find "${arkosSysroot}" -name 'libopenal*' -o -name 'al.h' 2>/dev/null | head -30 >&2 || true
            exit 1
          fi
          cmakeFlagsArray+=(
            "-DOPENAL_LIBRARY=$OPENAL_LIB"
            "-DOPENAL_INCLUDE_DIR=$OPENAL_INC"
          )

          MODPLUG_LIB=
          MODPLUG_INC=
          for cand in \
            "${arkosSysroot}/usr/lib/aarch64-linux-gnu/libmodplug.so" \
            "${arkosSysroot}/usr/lib/libmodplug.so" \
            "${arkosSysroot}/lib/aarch64-linux-gnu/libmodplug.so" \
            "${arkosSysroot}/usr/lib/aarch64-linux-gnu/libmodplug.so.1" \
            "${arkosSysroot}/usr/lib/libmodplug.so.1"
          do
            if [ -e "$cand" ]; then MODPLUG_LIB="$cand"; break; fi
          done
          for cand in \
            "${arkosSysroot}/usr/include" \
            "${arkosSysroot}/usr/include/aarch64-linux-gnu"
          do
            if [ -f "$cand/libmodplug/modplug.h" ] || [ -f "$cand/modplug.h" ]; then
              MODPLUG_INC="$cand"; break
            fi
          done
          if [ -z "$MODPLUG_LIB" ] || [ -z "$MODPLUG_INC" ]; then
            echo "arkos-sysroot: libmodplug required for enableSound but not found" >&2
            find "${arkosSysroot}" -name 'libmodplug*' -o -name 'modplug.h' 2>/dev/null | head -30 >&2 || true
            exit 1
          fi
          cmakeFlagsArray+=(
            "-DMODPLUG_LIBRARY=$MODPLUG_LIB"
            "-DMODPLUG_INCLUDE_DIRECTORY=$MODPLUG_INC"
          )
        fi
      '';

      postInstall = ''
        mkdir -p $out/share/windstille $out/lib/windstille
        # CMake may have installed data/ as non-writable; ensure we can add files.
        chmod -R u+w $out/share/windstille || true
        if [ -d "$src/data" ]; then
          cp -a "$src/data/." $out/share/windstille/ || true
          chmod -R u+w $out/share/windstille || true
        fi

        # Ship OpenAL Soft + libmodplug next to the port so stock ArkOS
        # (without apt install) can load them via LD_LIBRARY_PATH.
        if [ "${if enableSound then "1" else "0"}" = "1" ]; then
          for pattern in libopenal.so* libmodplug.so*; do
            found=
            for dir in \
              "${arkosSysroot}/usr/lib/aarch64-linux-gnu" \
              "${arkosSysroot}/usr/lib" \
              "${arkosSysroot}/lib/aarch64-linux-gnu" \
              "${arkosSysroot}/lib"
            do
              for f in "$dir"/$pattern; do
                if [ -e "$f" ]; then
                  cp -a "$f" "$out/lib/windstille/"
                  found=1
                fi
              done
            done
            if [ -z "$found" ]; then
              echo "postInstall: missing $pattern in sysroot" >&2
              exit 1
            fi
          done
          # Prefer real .so files over broken relative symlinks in the copy.
          chmod -R u+w "$out/lib/windstille" || true
          ls -la "$out/lib/windstille" || true
        fi
        cat > $out/share/windstille/README-R36S.txt << EOF_README
Windstille — R36S / ArkOS (sysroot-linked)
====================================================

Binary: bin/windstille
  Linked against the ArkOS aarch64 sysroot (SDL2 + OpenGL/GLES as available).

Deploy the binary + share/windstille data to the device.

Controls (important)
--------------------
Windstille uses the SDL Joystick API (raw indices). PortMaster still exports
SDL_GAMECONTROLLERCONFIG for other ports and for a future GameController path.

  Preferred: nix build .#windstille-r36s-portmaster
  and install under /roms/ports/ (launcher sources PortMaster control.txt).

  Manual SSH runs need no mapping for basic joystick input. See
  mk/r36s/CROSSCOMPILE.md for GLES env tips and exit-hotkey helpers..
EOF_README
        cat > $out/share/windstille/windstille.sh << 'LAUNCH'
#!/bin/bash
DIR="$(cd "$(dirname "$0")" && pwd)"
cd "$DIR"
BIN="$DIR/../bin/windstille"
if [ ! -x "$BIN" ]; then BIN="$DIR/windstille"; fi
# Without PortMaster control.txt, set SDL_GAMECONTROLLERCONFIG for GO-Super
# or the pad stays joystick-only (see mk/r36s/CROSSCOMPILE.md).
if [ -d "$DIR/libs" ]; then
  export LD_LIBRARY_PATH="$DIR/libs''${LD_LIBRARY_PATH:+:$LD_LIBRARY_PATH}"
elif [ -d "$DIR/../lib/windstille" ]; then
  export LD_LIBRARY_PATH="$DIR/../lib/windstille''${LD_LIBRARY_PATH:+:$LD_LIBRARY_PATH}"
fi
# Windstille options only (no Pingus --renderer / --software-cursor / --userdir).
# Force native panel size; defaults are desktop 1280x800.
exec "$BIN" \
  --geometry 640x480 \
  --fullscreen \
  --controller "$DIR/data/controller/r36s.scm" \
  "$@"
LAUNCH
        chmod +x $out/share/windstille/windstille.sh
      '';

      meta = with lib; {
        description = "Windstille for R36S/ArkOS (sysroot-linked aarch64)";
        license = licenses.gpl3Plus;
        platforms = platforms.linux;
        hydraPlatforms = [];
      };
    };

  # Wrap a mkWindstilleR36s result as a PortMaster-ready tree:
  #   Windstille.sh + windstille/ + metadata
  # Copy into /roms/ports/ or zip for PortMaster autoinstall.
  mkWindstilleR36sPortMaster = {
    r36sPkg
  , version
  , pname ? "windstille-r36s-portmaster"
  , title ? "Windstille"
  , scriptName ? "Windstille.sh"
  , portDirName ? "windstille"
  , screenshotSrc ? ../data/icon.png
  }:
    stdenvNoCC.mkDerivation {
      inherit pname version;
      dontUnpack = true;
      dontConfigure = true;
      dontBuild = true;
      # Device must run the launcher with ArkOS /bin/bash — never rewrite
      # shebangs to a nix store path (causes "bad interpreter" on R36S).
      dontPatchShebangs = true;
      dontFixup = true;

      # Binary is already cross-built; this is only packaging.
      nativeBuildInputs = [ ];

      installPhase = ''
        set -euo pipefail
        root="$out"
        gamedir="$root/${portDirName}"
        mkdir -p "$gamedir/data" "$gamedir/licenses" "$gamedir/conf"

        # Real ELF in bin/ (WINDSTILLE_DEFAULT_DATADIR is baked in; PortMaster passes --datadir).
        if [ -x "${r36sPkg}/bin/windstille" ]; then
          install -m755 "${r36sPkg}/bin/windstille" "$gamedir/windstille"
        else
          echo "portmaster: no windstille binary under ${r36sPkg}/bin" >&2
          ls -la "${r36sPkg}/bin" 2>/dev/null || true
          head -3 "${r36sPkg}/bin/windstille" 2>/dev/null || true
          exit 1
        fi

        # Bundled OpenAL / modplug (from r36s package, originally sysroot).
        if [ -d "${r36sPkg}/lib/windstille" ]; then
          mkdir -p "$gamedir/libs"
          cp -a "${r36sPkg}/lib/windstille/." "$gamedir/libs/"
          chmod -R u+w "$gamedir/libs" || true
        fi

        # Game data (CMake DATA_PREFIX was share/windstille).
        # Store paths are mode 444/555; make writable so we can drop helpers.
        if [ -d "${r36sPkg}/share/windstille" ]; then
          cp -a "${r36sPkg}/share/windstille/." "$gamedir/data/"
          chmod -R u+w "$gamedir/data"
          # Keep only game assets under data/; drop packaging helpers
          rm -f "$gamedir/data/windstille.sh" \
                "$gamedir/data/README-R36S.txt"
        fi

        # Placeholder / project icon as screenshot + cover (PortMaster wants
        # 4:3 gameplay ideally; icon is fine until a real capture is added).
        if [ -f "${screenshotSrc}" ]; then
          cp -f "${screenshotSrc}" "$root/screenshot.png"
          cp -f "${screenshotSrc}" "$root/cover.png"
          cp -f "${screenshotSrc}" "$gamedir/cover.png"
        else
          # Minimal valid 1x1 PNG if the asset is missing
          printf '\x89PNG\r\n\x1a\n\x00\x00\x00\rIHDR\x00\x00\x00\x01\x00\x00\x00\x01\x08\x02\x00\x00\x00\x90wS\xde\x00\x00\x00\x0cIDATx\x9cc\xf8\x0f\x00\x00\x01\x01\x00\x05\x18\xd8N\x00\x00\x00\x00IEND\xaeB`\x82' \
            > "$root/screenshot.png"
          cp -f "$root/screenshot.png" "$root/cover.png"
          cp -f "$root/screenshot.png" "$gamedir/cover.png"
        fi

        # License stubs
        if [ -d "${r36sPkg}/share/licenses" ]; then
          cp -a "${r36sPkg}/share/licenses/." "$gamedir/licenses/" || true
        fi
        cat > "$gamedir/licenses/README.txt" << 'EOF_LIC'
Windstille — see upstream GPL-3.0-or-later and LICENSES/ in the source tree.
This PortMaster package redistributes the game binary and data for ArkOS/R36S.
EOF_LIC

        # PortMaster launch script (sources control.txt → SDL_GAMECONTROLLERCONFIG)
        cat > "$root/${scriptName}" << 'EOF_LAUNCH'
#!/bin/bash
# Windstille — PortMaster launcher for R36S / ArkOS

XDG_DATA_HOME=''${XDG_DATA_HOME:-$HOME/.local/share}

if [ -d "/opt/system/Tools/PortMaster/" ]; then
  controlfolder="/opt/system/Tools/PortMaster"
elif [ -d "/opt/tools/PortMaster/" ]; then
  controlfolder="/opt/tools/PortMaster"
elif [ -d "$XDG_DATA_HOME/PortMaster/" ]; then
  controlfolder="$XDG_DATA_HOME/PortMaster"
else
  controlfolder="/roms/ports/PortMaster"
fi

source "$controlfolder/control.txt"
[ -f "''${controlfolder}/mod_''${CFW_NAME}.txt" ] && source "''${controlfolder}/mod_''${CFW_NAME}.txt"
get_controls

GAMEDIR="/$directory/ports/windstille"
CONFDIR="$GAMEDIR/conf"

mkdir -p "$CONFDIR"
# SD/FAT copies can leave files non-writable for user "ark"
chmod -R u+rwX "$GAMEDIR" 2>/dev/null || true
cd "$GAMEDIR" || exit 1

if [ -w "$GAMEDIR" ]; then
  > "$GAMEDIR/log.txt" 2>/dev/null && exec > >(tee -a "$GAMEDIR/log.txt") 2>&1 || true
fi

export XDG_DATA_HOME="$CONFDIR"
export XDG_CONFIG_HOME="$CONFDIR"
export SDL_GAMECONTROLLERCONFIG="$sdl_controllerconfig"
# Prefer bundled OpenAL Soft / libmodplug over any system copy.
if [ -d "$GAMEDIR/libs" ]; then
  export LD_LIBRARY_PATH="$GAMEDIR/libs''${LD_LIBRARY_PATH:+:$LD_LIBRARY_PATH}"
fi

# Native aarch64 SDL2 joystick input; gptokeyb optional for exit hotkey.
pm_platform_helper "$GAMEDIR/windstille" 2>/dev/null || true

# Force on-device data + config dirs (do not use any baked-in install prefix).
# Userdir comes from XDG_CONFIG_HOME above (no --userdir flag on Windstille).
# Geometry matches the R36S 640x480 panel; do not pass Pingus-only flags
# (--renderer, --software-cursor, --userdir).
./windstille \
  --datadir "$GAMEDIR/data" \
  --geometry 640x480 \
  --fullscreen \
  --controller "$GAMEDIR/data/controller/r36s.scm" \
  "$@"
pm_finish 2>/dev/null || true
EOF_LAUNCH
        chmod +x "$root/${scriptName}"

        # port.json (PortMaster catalog / autoinstall metadata)
        cat > "$root/port.json" << EOF_JSON
{
  "version": 2,
  "name": "windstille.zip",
  "items": [
    "${scriptName}",
    "${portDirName}"
  ],
  "items_opt": null,
  "attr": {
    "title": "${title}",
    "desc": "Classic Windstille (SDL2 + GLES2) for ArkOS / R36S. Free jump-and-run platformer starring Tux.",
    "inst": "Ready to run. Copy Windstille.sh and the windstille/ folder into /roms/ports/ (or install the zip via PortMaster autoinstall).",
    "genres": ["platform", "action"],
    "porter": ["Windstille-Origins"],
    "image": {},
    "rtr": true,
    "runtime": null,
    "reqs": [],
    "arch": ["aarch64"]
  }
}
EOF_JSON

        # EmulationStation metadata (cover used as list art)
        cat > "$root/gameinfo.xml" << EOF_XML
<?xml version="1.0" encoding="utf-8"?>
<gameList>
  <game>
    <path>./${scriptName}</path>
    <name>${title}</name>
    <desc>Classic Windstille — free jump-and-run platformer starring Tux. SDL2 + GLES2 build for ArkOS / R36S.</desc>
    <releasedate>20040511T000000</releasedate>
    <developer>Windstille Team</developer>
    <publisher>Windstille-Origins</publisher>
    <genre>Platform</genre>
    <image>./${portDirName}/cover.png</image>
  </game>
</gameList>
EOF_XML

        cat > "$root/README.md" << 'EOF_README'
## Windstille (R36S / ArkOS)

Native **aarch64** build linked against the ArkOS sysroot (SDL2 + GLES2).

### Install

1. Copy `Windstille.sh` and the `windstille/` directory to `/roms/ports/` on the device, **or**
2. Zip this folder and place the zip in `ports/PortMaster/autoinstall/`, then open PortMaster once.

### Controls (required)

Windstille reads the pad via the **SDL Joystick API** (raw button/axis indices).
`SDL_GAMECONTROLLERCONFIG` is still exported by this launcher for consistency
with other PortMaster ports; it does not remap Windstille’s joystick bindings.

**This PortMaster launcher** sources `control.txt` (`get_controls`). Prefer
launching via EmulationStation **Ports** (or the `.sh` script). Select+Start
exit is handled by gptokeyb/oga_controls when the launcher starts them — not
by Windstille itself.

In-game: D-pad / left stick move; face buttons follow the device’s joystick
indices (see `mk/r36s/CROSSCOMPILE.md`).

### Credits

Thanks to the Windstille developers and the PortMaster / ArkOS communities.
EOF_README

        '';

      meta = with lib; {
        description = "PortMaster package of Windstille for R36S/ArkOS";
        license = licenses.gpl3Plus;
        platforms = platforms.linux;
        hydraPlatforms = [];
      };
    };

  # Zip of the PortMaster tree for autoinstall (ports/PortMaster/autoinstall/).
  # Flat archive: launcher + port dir + metadata at zip root (matches port.json).
  mkWindstilleR36sPortMasterZip = {
    portMasterPkg
  , version
  , pname ? "windstille-r36s-portmaster-zip"
  , zipName ? "windstille.zip"
  }:
    stdenvNoCC.mkDerivation {
      inherit pname version;
      dontUnpack = true;
      dontConfigure = true;
      dontBuild = true;
      dontFixup = true;
      dontPatchShebangs = true;

      nativeBuildInputs = [ zip ];

      installPhase = ''
        set -euo pipefail
        mkdir -p "$out"
        ( cd "${portMasterPkg}" && zip -r -9 "$out/${zipName}" . )
      '';

      meta = with lib; {
        description = "PortMaster autoinstall zip of Windstille for R36S/ArkOS";
        license = licenses.gpl3Plus;
        platforms = platforms.linux;
        hydraPlatforms = [];
      };
    };

  # --- Exception smoke test (throw/catch) against the ArkOS sysroot -----------
  # Build-time discovery only (no pathExists / IFD).
  # - pthread shim wins over GCC include-fixed (glibc 2.32+ types vs ArkOS 2.30)
  # - arkos_compat.c provides _dl_find_object for static libgcc_eh
  # - libstdc++ headers may live under the gcc out path or a sibling; search both

  mkExceptionTest = {
    label ? "current"
  , gcc ? resolveCrossGcc label
  , pname ? "r36s-exception-test-${label}"
  }:
    assert gcc != null;
    let
      ver = gcc.version or label;
      libdir = "${arkosSysroot}/usr/lib/aarch64-linux-gnu";
      sysroot = arkosSysroot;
      gccLibOut = lib.getLib gcc;
      # Force <pthread.h> to the ArkOS header so GCC include-fixed (which needs
      # bits/types/struct___jmp_buf_tag.h from glibc ≥2.32) is never used.
      pthreadShim = writeTextFile {
        name = "arkos-pthread-shim";
        destination = "/pthread.h";
        text = ''
          /* Prefer ArkOS sysroot pthread; do not use GCC include-fixed. */
          #include "${sysroot}/usr/include/pthread.h"
        '';
      };
    in
    stdenvNoCC.mkDerivation {
      inherit pname;
      version = ver;
      dontUnpack = true;
      dontConfigure = true;
      strictDeps = true;
      nativeBuildInputs = [ crossCc.bintools ];

      buildPhase = ''
        runHook preBuild
        set -euo pipefail
        echo "==> exception_test label=${label} gcc=${ver}"

        GCC_ROOT="${gcc}"
        VER="${ver}"
        SYSROOT="${sysroot}"
        LIBDIR="${libdir}"

        # --- locate gcc / g++ ---
        # arkos_compat.c must be compiled as C so _dl_find_object is not mangled.
        GXX=
        for cand in "$GCC_ROOT/bin/${targetPrefix}g++" "$GCC_ROOT/bin/g++"; do
          if [ -x "$cand" ]; then GXX="$cand"; break; fi
        done
        if [ -z "$GXX" ]; then
          echo "exception-test: no g++ under $GCC_ROOT/bin" >&2
          ls -la "$GCC_ROOT/bin" >&2 || true
          exit 1
        fi
        GCC=
        for cand in "$GCC_ROOT/bin/${targetPrefix}gcc" "$GCC_ROOT/bin/gcc"; do
          if [ -x "$cand" ]; then GCC="$cand"; break; fi
        done
        if [ -z "$GCC" ]; then
          echo "exception-test: no gcc under $GCC_ROOT/bin" >&2
          exit 1
        fi

        # --- locate libstdc++ headers ---
        # Layouts seen in nixpkgs cross gcc:
        #   $out/include/c++/$VER/aarch64-…/bits/c++config.h          (newer)
        #   $out/aarch64-…/include/c++/$VER/aarch64-…/bits/c++config.h (gcc 12)
        CXX_INC=
        CXX_INC_TARGET=
        for root in "$GCC_ROOT" "${gccLibOut}"; do
          [ -d "$root" ] || continue
          for vdir in \
              "$root/include/c++/$VER" \
              "$root/include/c++"/* \
              "$root/aarch64-unknown-linux-gnu/include/c++/$VER" \
              "$root/aarch64-linux-gnu/include/c++/$VER" \
              "$root"/aarch64-*/include/c++/$VER \
              "$root"/aarch64-*/include/c++/*
          do
            [ -d "$vdir" ] || continue
            for t in aarch64-unknown-linux-gnu aarch64-linux-gnu; do
              if [ -f "$vdir/$t/bits/c++config.h" ]; then
                CXX_INC="$vdir"
                CXX_INC_TARGET="$vdir/$t"
                break 3
              fi
            done
            cfg=$(find "$vdir" -path '*/aarch64*/bits/c++config.h' 2>/dev/null | head -1 || true)
            if [ -n "$cfg" ]; then
              CXX_INC_TARGET=$(dirname "$(dirname "$cfg")")
              CXX_INC=$(dirname "$CXX_INC_TARGET")
              break 2
            fi
          done
        done
        # Last resort: find any aarch64 c++config under the gcc store path
        if [ -z "$CXX_INC_TARGET" ]; then
          cfg=$(find "$GCC_ROOT" "${gccLibOut}" -path '*/aarch64*/bits/c++config.h' 2>/dev/null | head -1 || true)
          if [ -n "$cfg" ]; then
            CXX_INC_TARGET=$(dirname "$(dirname "$cfg")")
            CXX_INC=$(dirname "$CXX_INC_TARGET")
          fi
        fi
        if [ -z "$CXX_INC_TARGET" ]; then
          echo "exception-test: no aarch64 bits/c++config.h under $GCC_ROOT (or lib out)" >&2
          find "$GCC_ROOT" "${gccLibOut}" -name 'c++config.h' 2>/dev/null | head -30 >&2 || true
          exit 1
        fi
        echo "    CXX_INC=$CXX_INC"
        echo "    CXX_INC_TARGET=$CXX_INC_TARGET"

        # --- libgcc dir (for -B / -L / static-libgcc) ---
        LIBGCC_DIR=
        FIXED_INC=
        for t in aarch64-unknown-linux-gnu aarch64-linux-gnu; do
          if [ -d "$GCC_ROOT/lib/gcc/$t/$VER/include" ]; then
            FIXED_INC="$GCC_ROOT/lib/gcc/$t/$VER/include"
            LIBGCC_DIR="$GCC_ROOT/lib/gcc/$t/$VER"
            break
          fi
        done
        if [ -z "$LIBGCC_DIR" ]; then
          FIXED_INC=$(find "$GCC_ROOT/lib/gcc" -type d -path '*/aarch64*/'"$VER"'/include' 2>/dev/null | head -1 || true)
          LIBGCC_DIR=$(dirname "$FIXED_INC")
        fi
        echo "    LIBGCC_DIR=$LIBGCC_DIR"

        stdcpp=
        for cand in \
          "$LIBDIR/libstdc++.so" \
          "$LIBDIR/libstdc++.so.6" \
          "$SYSROOT/usr/lib/libstdc++.so" \
          "$SYSROOT/usr/lib/libstdc++.so.6" \
          "$SYSROOT/lib/aarch64-linux-gnu/libstdc++.so" \
          "$SYSROOT/lib/aarch64-linux-gnu/libstdc++.so.6"
        do
          if [ -e "$cand" ]; then stdcpp="$cand"; break; fi
        done
        if [ -z "$stdcpp" ]; then
          echo "exception-test: no libstdc++ in sysroot" >&2
          exit 1
        fi

        # C flags (no C++ isystem / -nostdinc++) for arkos_compat.c
        COMPILE_C=(
          -nostdinc
          --sysroot="$SYSROOT"
          -isystem "${pthreadShim}"
          -isystem "$FIXED_INC"
          -isystem "$SYSROOT/usr/include/aarch64-linux-gnu"
          -isystem "$SYSROOT/usr/include"
          -pthread -fexceptions
          -march=armv8-a -mtune=cortex-a35
        )
        # C++ flags: pthread shim first so GCC never uses include-fixed/pthread.h
        COMPILE_CXX=(
          -nostdinc -nostdinc++
          -D_GLIBCXX_USE_CXX11_ABI=0
          -DR36S_EXC_TEST_LABEL='"${label}"'
          -DR36S_EXC_TEST_GCC_VER='"${ver}"'
          --sysroot="$SYSROOT"
          -isystem "${pthreadShim}"
          -isystem "$CXX_INC"
          -isystem "$CXX_INC_TARGET"
          -isystem "$CXX_INC/backward"
          -isystem "$FIXED_INC"
          -isystem "$SYSROOT/usr/include/aarch64-linux-gnu"
          -isystem "$SYSROOT/usr/include"
          -pthread -fexceptions -frtti
          -march=armv8-a -mtune=cortex-a35
        )

        # Compat object: compile as C so _dl_find_object is not C++-mangled.
        "$GCC" -B${crossCc.bintools}/bin "''${COMPILE_C[@]}" -c -o arkos_compat.o \
          ${../mk/r36s/arkos_compat.c}

        "$GXX" -B${crossCc.bintools}/bin "''${COMPILE_CXX[@]}" -c -o exception_test.o \
          ${../mk/r36s/exception_test.cpp}

        # arkos_compat.o after -static-libgcc so it resolves libgcc_eh's refs.
        "$GXX" -B${crossCc.bintools}/bin "''${COMPILE_CXX[@]}" \
          -nostdlib++ \
          --sysroot="$SYSROOT" \
          -Wl,--sysroot="$SYSROOT" \
          -Wl,--dynamic-linker=/lib/ld-linux-aarch64.so.1 \
          -B"$LIBDIR" \
          -B"$LIBGCC_DIR" \
          -L"$LIBDIR" \
          -L"$SYSROOT/usr/lib" \
          -L"$SYSROOT/lib" \
          -L"$SYSROOT/lib/aarch64-linux-gnu" \
          -L"$LIBGCC_DIR" \
          -L${gccLibOut}/lib \
          -static-libgcc \
          -Wl,--undefined=_dl_find_object \
          -Wl,--allow-shlib-undefined \
          -march=armv8-a -mtune=cortex-a35 \
          -o exception_test \
          exception_test.o \
          -Wl,--no-as-needed "$stdcpp" \
          -Wl,-Bdynamic -l:libpthread.so.0 -lm \
          -Wl,--as-needed \
          arkos_compat.o

        ${crossCc.bintools}/bin/${targetPrefix}readelf -h exception_test || true
        ${crossCc.bintools}/bin/${targetPrefix}readelf -d exception_test | head -30 || true
        runHook postBuild
      '';

      installPhase = ''
        runHook preInstall
        mkdir -p "$out/bin"
        cp -v exception_test "$out/bin/exception_test"
        cat > "$out/bin/README.txt" << EOF
R36S exception smoke test (${label}, GCC ${ver})

Copy exception_test to the device (or qemu-user with ArkOS rootfs):

  ./exception_test ; echo exit:\$?

Expect ALL PASSED. Abort in uw_init_context_1 means unwind still broken
for this compiler + ArkOS libstdc++/glibc pair.
EOF
        runHook postInstall
      '';

      meta = with lib; {
        description = "R36S/ArkOS C++ throw/catch smoke test (GCC ${label})";
        platforms = platforms.linux;
        hydraPlatforms = [];
      };
    };

  # Resolve aarch64 cross gcc for a label (no pathExists / IFD).
  resolveCrossGcc = label:
    let fromExtra = extraCrossGccs.${label} or null;
    in
    if label == "current" then crossCc.cc
    else if fromExtra != null then fromExtra
    else null;

  # Labels to try. Missing majors are skipped (null filtered out).
  exceptionTestLabels = [ "current" "14" "13" "12" ];

  exceptionTests =
    lib.filterAttrs (_: v: v != null) (
      lib.listToAttrs (
        map (label:
          let gcc = resolveCrossGcc label;
          in {
            name = label;
            value =
              if gcc == null then null
              else mkExceptionTest { inherit label gcc; };
          }
        ) exceptionTestLabels
      )
    );

  # One derivation that gathers every successfully built binary for on-device
  # comparison (copy the whole share/r36s-exception-tests/ directory).
  r36s-exception-tests = stdenvNoCC.mkDerivation {
    pname = "r36s-exception-tests";
    version = "0.1";
    dontUnpack = true;
    dontConfigure = true;
    dontBuild = true;
    # Depend on all matrix members so they build when this attr is requested.
    buildInputs = lib.attrValues exceptionTests;

    installPhase = ''
      runHook preInstall
      mkdir -p "$out/share/r36s-exception-tests"
      ${lib.concatMapStrings (label: ''
        mkdir -p "$out/share/r36s-exception-tests/${label}"
        cp -v ${exceptionTests.${label}}/bin/exception_test \
          "$out/share/r36s-exception-tests/${label}/exception_test"
        cp -v ${exceptionTests.${label}}/bin/README.txt \
          "$out/share/r36s-exception-tests/${label}/README.txt" 2>/dev/null || true
      '') (lib.attrNames exceptionTests)}
      cat > "$out/share/r36s-exception-tests/README.md" << 'EOF'
# R36S exception-test matrix

Each subdirectory is an aarch64 binary built with a different cross GCC
against the **same ArkOS sysroot** (libstdc++ / glibc ~2.30).

On the device (or qemu with that rootfs):

```sh
for d in */; do
  echo "=== $d ==="
  "$d/exception_test" && echo PASS || echo FAIL:$?
done
```

`current` is the toolchain used for `windstille-r36s` (often GCC 15).
`14` / `13` / `12` come from flake inputs `nixpkgs-24_11`, `nixpkgs-24_05`,
`nixpkgs-23_11` (each channel's aarch64 *cross* `stdenv.cc`, not native
`buildPackages.gccN` — those lack aarch64 `bits/c++config.h`).
EOF
      runHook postInstall
    '';

    meta = with lib; {
      description = "Matrix of R36S C++ exception smoke tests across GCC majors";
      platforms = platforms.linux;
      hydraPlatforms = [];
    };
  };

in
{
  inherit arkosSysroot mkWindstilleR36s mkWindstilleR36sPortMaster mkWindstilleR36sPortMasterZip;
  inherit mkExceptionTest exceptionTests r36s-exception-tests;
  # Convenience: default label (same GCC as the game build).
  r36s-exception-test = exceptionTests.current or (mkExceptionTest { label = "current"; });
}
