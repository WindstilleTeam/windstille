#!/usr/bin/env bash
# Builds the Windstille APK, linking SDL2 as a prebuilt library.
# Required environment:
#   ANDROID_HOME, BUILD_TOOLS_VERSION, PACKAGE_PLATFORM, TARGET_ABIS
#   APP_NAME, APP_DIR          - android/ packaging dir (manifest, res, jni/)
#   GAME_SRC_DIR               - path to C++ sources (repo src/)
#   GAME_DATA_DIR              - required data/ tree packaged as assets
#   APPLICATION_MK, TOP_ANDROID_MK, SDL_PREBUILT_MK, SDL_ANDROID_LIBS
#   KEYSTORE, STB_IMAGE_H
#   WINDSTILLE_VERSION           - full version string (e.g. 0.8.0-dev+gabc1234)
set -euo pipefail


# Resolve NDK root: ndk-bundle (legacy) or ndk/<version> (current SDK layout).
resolve_ndk() {
  if [ -n "${ANDROID_NDK_HOME:-}" ] && [ -x "${ANDROID_NDK_HOME}/ndk-build" ]; then
    printf '%s' "$ANDROID_NDK_HOME"
    return
  fi
  if [ -z "${ANDROID_HOME:-}" ]; then
    echo "error: ANDROID_HOME is not set" >&2
    exit 1
  fi
  if [ -x "$ANDROID_HOME/ndk-bundle/ndk-build" ]; then
    printf '%s' "$ANDROID_HOME/ndk-bundle"
    return
  fi
  if [ -d "$ANDROID_HOME/ndk" ]; then
    # Prefer ANDROID_NDK_VERSION when set; else newest directory that has ndk-build.
    if [ -n "${ANDROID_NDK_VERSION:-}" ] && [ -x "$ANDROID_HOME/ndk/$ANDROID_NDK_VERSION/ndk-build" ]; then
      printf '%s' "$ANDROID_HOME/ndk/$ANDROID_NDK_VERSION"
      return
    fi
    newest=
    for d in "$ANDROID_HOME/ndk"/*; do
      [ -x "$d/ndk-build" ] || continue
      newest=$d
    done
    if [ -n "$newest" ]; then
      printf '%s' "$newest"
      return
    fi
  fi
  echo "error: no ndk-build under ANDROID_HOME=$ANDROID_HOME (tried ndk-bundle and ndk/*)" >&2
  exit 1
}

NDK="$(resolve_ndk)"
echo "==> NDK=$NDK"
BT="$ANDROID_HOME/build-tools/$BUILD_TOOLS_VERSION"
PACKAGE_JAR="$ANDROID_HOME/platforms/android-$PACKAGE_PLATFORM/android.jar"

if [ -z "${GAME_SRC_DIR:-}" ] || [ ! -d "$GAME_SRC_DIR" ]; then
  echo "error: GAME_SRC_DIR must point at the game C++ source tree" >&2
  exit 1
fi

if [ -z "${GAME_DATA_DIR:-}" ] || [ ! -d "$GAME_DATA_DIR" ]; then
  echo "error: GAME_DATA_DIR must point at the Windstille data/ tree" >&2
  echo "       (expected images/, levels/, levelsets/, music/, … under that path)" >&2
  exit 1
fi

mkdir -p src/jni/src src/jni/SDL
cp "$APPLICATION_MK" src/jni/Application.mk
cp "$TOP_ANDROID_MK" src/jni/Android.mk
cp "$APP_DIR/jni/Android.mk" src/jni/src/Android.mk
cp "$APP_DIR/AndroidManifest.xml" src/AndroidManifest.xml
cp -r "$APP_DIR/res" src/res

# Game C++ sources next to the module Android.mk.
cp -r "$GAME_SRC_DIR"/. src/jni/src/
chmod -R u+rwX src/jni/src

# Generate Squirrel wrapper (normally a CMake custom target on desktop).
if [ -n "${MINISWIG:-}" ] && [ -x "$MINISWIG" ]; then
  echo "==> generating squirrel/wrapper via miniswig"
  mkdir -p src/jni/src/squirrel "$TMPDIR/miniswig"
  WRAP_IFACE="src/jni/src/scripting/wrapper.interface.hpp"
  if [ ! -f "$WRAP_IFACE" ]; then
    echo "error: missing $WRAP_IFACE" >&2
    exit 1
  fi
  # Preprocess like desktop CMake ( -x c -CC keeps comments for miniswig )
  ${CXX:-g++} -E -I src/jni/src -x c -CC "$WRAP_IFACE" -o "$TMPDIR/miniswig/miniswig.tmp" -DSCRIPTING_API
  "$MINISWIG"     --input "$TMPDIR/miniswig/miniswig.tmp"     --output-cpp src/jni/src/squirrel/wrapper.cpp     --output-hpp src/jni/src/squirrel/wrapper.hpp     --module windstille     --select-namespace "Scripting"
  ls -la src/jni/src/squirrel/wrapper.*
else
  echo "warning: MINISWIG unset — squirrel/wrapper.hpp will be missing" >&2
fi

# Stage monorepo external/ headers + sources.
# Under Nix, GAME_SRC_DIR is a filtered ./src store path — parent is NOT the
# repo. Pass GAME_EXTERNAL_DIR (flake: ./external) and optional GLM_INCLUDE_DIR.
EXTERNAL_DIR="${GAME_EXTERNAL_DIR:-}"
if [ -z "$EXTERNAL_DIR" ]; then
  REPO_ROOT="$(cd "$GAME_SRC_DIR/.." && pwd)"
  if [ -d "$REPO_ROOT/external" ]; then
    EXTERNAL_DIR="$REPO_ROOT/external"
  fi
fi
if [ -z "$EXTERNAL_DIR" ] || [ ! -d "$EXTERNAL_DIR" ]; then
  echo "error: GAME_EXTERNAL_DIR must point at the repo external/ tree" >&2
  echo "       (contains geomcpp/, prio/, sexpcpp/, logmich/, …)" >&2
  exit 1
fi

mkdir -p src/jni/external_includes
# Nix store trees are often 0555/0444. cp -a preserves that and the next
# package cannot create e.g. external_includes/geom → Permission denied.
# Copy then force owner-writable on the staging tree.
# Header-only / public includes (layout: include/<ns>/… → external_includes/<ns>/…)
# dir names in Windstille external/ (prio, not priocpp). optional → warn only.
for entry in argpp:req geomcpp:req logmich:req prio:req strutcpp:req sexpcpp:req              babyxml:req surfcpp:req wstdisplay:req wstinput:req wstsound:req              wstgui:opt tinygettext:opt; do
  name="${entry%%:*}"
  mode="${entry##*:}"
  inc="$EXTERNAL_DIR/$name/include"
  if [ -d "$inc" ]; then
    cp -a "$inc"/. src/jni/external_includes/
    chmod -R u+rwX src/jni/external_includes
  elif [ "$mode" = "opt" ]; then
    echo "warning: optional external missing $inc" >&2
  else
    echo "error: missing $inc" >&2
    exit 1
  fi
done
# glm is header-only (geom depends on it).
if [ -n "${GLM_INCLUDE_DIR:-}" ] && [ -d "$GLM_INCLUDE_DIR" ]; then
  # Expect GLM_INCLUDE_DIR to contain glm/… (nixpkgs glm) or be the glm/ dir itself.
  if [ -d "$GLM_INCLUDE_DIR/glm" ]; then
    cp -a "$GLM_INCLUDE_DIR/glm" src/jni/external_includes/
  elif [ "$(basename "$GLM_INCLUDE_DIR")" = "glm" ]; then
    cp -a "$GLM_INCLUDE_DIR" src/jni/external_includes/
  else
    echo "error: GLM_INCLUDE_DIR=$GLM_INCLUDE_DIR does not look like glm headers" >&2
    exit 1
  fi
  chmod -R u+rwX src/jni/external_includes
  echo "==> staged glm headers from $GLM_INCLUDE_DIR"
else
  echo "error: GLM_INCLUDE_DIR is required for Android (geom → glm)" >&2
  exit 1
fi
echo "==> staged external headers into jni/external_includes"
# tinygettext ships a Windows dirent.h; if it sits on -I, <dirent.h> resolves
# to it even on Android. Always drop the Windows shim from the stage tree.
rm -f src/jni/external_includes/tinygettext/dirent.h

# Compile external .cpp into libmain (ndk-build RWILDCARD under jni/src/).
# Skip tests/benchmarks; skip priocpp JSON (no jsoncpp on Android).
mkdir -p src/jni/src/deps
stage_lib_src() {
  local name="$1"
  local srcdir="$EXTERNAL_DIR/$name/src"
  if [ ! -d "$srcdir" ]; then
    echo "warning: no sources for $name ($srcdir)" >&2
    return 0
  fi
  mkdir -p "src/jni/src/deps/$name"
  # Recursive sources (wstdisplay/scenegraph/…, etc.) + private headers.
  find "$srcdir" -type f \( -name '*.cpp' -o -name '*.c' -o -name '*.hpp' -o -name '*.h' \)     ! -path '*/test/*' ! -path '*/tests/*' ! -path '*/extra/*' | while read -r f; do
      rel="${f#"$srcdir"/}"
      mkdir -p "src/jni/src/deps/$name/$(dirname "$rel")"
      cp -a "$f" "src/jni/src/deps/$name/$rel"
    done
  chmod -R u+rwX "src/jni/src/deps/$name"
}
stage_lib_src argpp
stage_lib_src logmich
stage_lib_src sexpcpp
stage_lib_src strutcpp
stage_lib_src prio
stage_lib_src babyxml
stage_lib_src surfcpp
stage_lib_src wstdisplay
stage_lib_src wstinput
# Android.mk references deps/priocpp — alias sources staged as prio.
if [ -d src/jni/src/deps/prio ] && [ ! -e src/jni/src/deps/priocpp ]; then
  ln -s prio src/jni/src/deps/priocpp
fi
stage_lib_src tinygettext
# Drop JSON backends (PRIO_USE_JSONCPP is off).
rm -f src/jni/src/deps/prio/json_*.cpp \
      src/jni/src/deps/prio/jsonpretty_*.cpp \
      src/jni/src/deps/priocpp/json_*.cpp \
      src/jni/src/deps/priocpp/jsonpretty_*.cpp
# strut layout.cpp needs a missing polygon.hpp; Windstille does not use Layout.
rm -f src/jni/src/deps/strutcpp/layout.cpp
echo "==> staged external sources into jni/src/deps/"

# Optional Squirrel (headers + sources compiled into libmain).
if [ -n "${SQUIRREL_SRC:-}" ]; then
  echo "==> staging Squirrel from $SQUIRREL_SRC"
  mkdir -p src/jni/external_includes src/jni/src/deps/squirrel "$TMPDIR/squirrel-src"
  SQROOT="$SQUIRREL_SRC"
  if [ -f "$SQUIRREL_SRC" ]; then
    # fetchurl store path is a tarball
    mkdir -p "$TMPDIR/squirrel-unpack"
    tar -xzf "$SQUIRREL_SRC" -C "$TMPDIR/squirrel-unpack"
    SQROOT="$(find "$TMPDIR/squirrel-unpack" -maxdepth 2 -type d -name squirrel -printf '%h
' | head -1)"
    if [ -z "$SQROOT" ]; then
      SQROOT="$(find "$TMPDIR/squirrel-unpack" -maxdepth 1 -type d ! -path "$TMPDIR/squirrel-unpack" | head -1)"
    fi
  fi
  if [ -d "$SQROOT/include" ]; then
    cp -a "$SQROOT/include"/. src/jni/external_includes/
  fi
  # ensure squirrel.h on -I path (not only include/squirrel/)
  if [ -f src/jni/external_includes/squirrel.h ]; then
    :
  elif [ -f src/jni/external_includes/squirrel/squirrel.h ]; then
    cp -a src/jni/external_includes/squirrel/squirrel.h src/jni/external_includes/
  fi
  for sub in squirrel sqstdlib; do
    if [ -d "$SQROOT/$sub" ]; then
      find "$SQROOT/$sub" -maxdepth 1 -name '*.cpp' -exec cp -a {} src/jni/src/deps/squirrel/ \;
    fi
  done
  chmod -R u+rwX src/jni/external_includes src/jni/src/deps/squirrel
  echo "==> Squirrel headers: $(ls src/jni/external_includes/squirrel.h 2>/dev/null || echo MISSING)"
fi
# Optional FreeType (headers + static libs per ABI under FREETYPE_ANDROID_LIBS).
if [ -n "${FREETYPE_ANDROID_LIBS:-}" ] && [ -d "$FREETYPE_ANDROID_LIBS" ]; then
  echo "==> staging FreeType from $FREETYPE_ANDROID_LIBS"
  mkdir -p src/jni/freetype
  cp -a "$FREETYPE_ANDROID_LIBS"/. src/jni/freetype/
  chmod -R u+rwX src/jni/freetype
  # CMake installs ft2build.h under include/freetype2/
  if [ ! -f src/jni/freetype/include/ft2build.h ]; then
    if [ -f src/jni/freetype/include/freetype2/ft2build.h ]; then
      # Prefer -I include/freetype2 (also keep include/)
      ln -s freetype2/ft2build.h src/jni/freetype/include/ft2build.h ||         cp src/jni/freetype/include/freetype2/ft2build.h src/jni/freetype/include/
    fi
  fi
  echo "==> FreeType ft2build: $(ls src/jni/freetype/include/ft2build.h src/jni/freetype/include/freetype2/ft2build.h 2>/dev/null || echo MISSING)"
fi


# wstsound (wav + modplug only; match EMSCRIPTEN/ANDROID CMake defaults)
if [ -d "$EXTERNAL_DIR/wstsound/src" ]; then
  mkdir -p src/jni/src/deps/wstsound
  find "$EXTERNAL_DIR/wstsound/src" -maxdepth 1 -name '*.cpp' -exec cp -a {} src/jni/src/deps/wstsound/ \;
  find "$EXTERNAL_DIR/wstsound/src" -maxdepth 1 \( -name '*.hpp' -o -name '*.h' \) -exec cp -a {} src/jni/src/deps/wstsound/ \;
  # Drop codecs / EFX not used on Android
  rm -f src/jni/src/deps/wstsound/ogg_sound_file.cpp \
        src/jni/src/deps/wstsound/opus_sound_file.cpp \
        src/jni/src/deps/wstsound/mp3_sound_file.cpp \
        src/jni/src/deps/wstsound/effect.cpp \
        src/jni/src/deps/wstsound/effect_slot.cpp \
        src/jni/src/deps/wstsound/filter.cpp \
        src/jni/src/deps/wstsound/procedural_sound_file.cpp
  # Public headers: include/wstsound/*.hpp
  if [ -d "$EXTERNAL_DIR/wstsound/include/wstsound" ]; then
    mkdir -p src/jni/external_includes/wstsound
    cp -a "$EXTERNAL_DIR/wstsound/include/wstsound/." src/jni/external_includes/wstsound/
  fi
  chmod -R u+rwX src/jni/src/deps/wstsound src/jni/external_includes/wstsound 2>/dev/null || true
  echo "==> staged wstsound (wav+modplug) into jni/src/deps/wstsound"
fi

# Prebuilt OpenAL Soft + libmodplug (from nix audioAndroidLibs)
ENABLE_ANDROID_SOUND=0
if [ -n "${AUDIO_ANDROID_LIBS:-}" ] && [ -d "$AUDIO_ANDROID_LIBS" ]; then
  mkdir -p src/jni/audio
  # Layout: jni/audio/<abi>/lib/*.a and shared jni/audio/include/
  # Headers are identical per ABI; copy once and chmod writable (nix store is 0444).
  headers_done=0
  for abi_dir in "$AUDIO_ANDROID_LIBS"/*; do
    [ -d "$abi_dir" ] || continue
    abi=$(basename "$abi_dir")
    case "$abi" in
      armeabi-v7a|arm64-v8a|x86|x86_64)
        mkdir -p "src/jni/audio/$abi/lib"
        cp -a "$abi_dir"/lib/*.a "src/jni/audio/$abi/lib/" 2>/dev/null || true
        chmod -R u+w "src/jni/audio/$abi/lib" 2>/dev/null || true
        if [ "$headers_done" -eq 0 ] && [ -d "$abi_dir/include" ]; then
          mkdir -p src/jni/audio/include
          cp -a "$abi_dir/include/." src/jni/audio/include/
          chmod -R u+rwX src/jni/audio/include
          headers_done=1
        fi
        ;;
    esac
  done
  if ls src/jni/audio/*/lib/libopenal.a >/dev/null 2>&1 &&      ls src/jni/audio/*/lib/libmodplug.a >/dev/null 2>&1; then
    ENABLE_ANDROID_SOUND=1
    echo "==> staged OpenAL Soft + libmodplug prebuilts (sound enabled)"
  else
    echo "warning: AUDIO_ANDROID_LIBS set but openal/modplug .a missing — sound disabled" >&2
    find src/jni/audio -type f 2>/dev/null | head -20 >&2 || true
  fi
else
  echo "==> AUDIO_ANDROID_LIBS not set — building with WINDSTILLE_NO_SOUND"
fi
export ENABLE_ANDROID_SOUND

# Minimal sigc++ headers (Windstille Android polyfill — full libsigc++ not required).
mkdir -p src/jni/external_includes/sigc++
cp -a "$APP_DIR/jni/sigc++/." src/jni/external_includes/sigc++/
chmod -R u+rwX src/jni/external_includes/sigc++
echo "==> staged Android sigc++ polyfill"

# IMG_* shim + headers.
cp "$APP_DIR/jni/img_stb_min.c" src/jni/src/img_stb_min.c
cp "$APP_DIR/jni/android_SDL_image.h" src/jni/src/SDL_image.h
if [ -n "${STB_IMAGE_H:-}" ] && [ -f "$STB_IMAGE_H" ]; then
  cp "$STB_IMAGE_H" src/jni/src/stb_image.h
elif [ -f "$APP_DIR/jni/stb_image.h" ]; then
  cp "$APP_DIR/jni/stb_image.h" src/jni/src/stb_image.h
else
  echo "error: need STB_IMAGE_H or android/jni/stb_image.h (upstream stb)" >&2
  exit 1
fi

cp "$SDL_PREBUILT_MK" src/jni/SDL/Android.mk
cp -r "$SDL_ANDROID_LIBS/include" src/jni/SDL/include
# SDL2_mixer is optional (Windstille uses OpenAL + modplug via AUDIO_ANDROID_LIBS).
if [ -f src/jni/SDL/include/SDL_mixer.h ] || [ -f src/jni/SDL/include/SDL2/SDL_mixer.h ]; then
  echo "SDL2_mixer headers present"
fi

# Game data → APK assets/ (AssetManager root).
mkdir -p src/assets
cp -a "$GAME_DATA_DIR"/. src/assets/
# Nix store files are often 0444; aapt/zip need readable tree we can scan.
chmod -R u+rwX src

ASSET_COUNT=$(find src/assets -type f | wc -l)
ASSET_SIZE=$(du -sh src/assets | awk '{print $1}')
echo "Packaging $ASSET_COUNT asset files ($ASSET_SIZE) from $GAME_DATA_DIR"

# Flat list of asset paths so native code can "opendir" without AAssetManager_list.
# Paths are relative to AssetManager root (same as SDL_RWFromFile).
( cd src/assets && find . -type f ! -name 'android-asset-index.txt' | sed 's|^\./||' | sort > android-asset-index.txt )
INDEX_COUNT=$(wc -l < src/assets/android-asset-index.txt | tr -d ' ')
echo "Wrote android-asset-index.txt ($INDEX_COUNT paths)"

if [ "$ASSET_COUNT" -lt 10 ]; then
  echo "error: asset tree looks empty (found $ASSET_COUNT files)" >&2
  ls -la src/assets >&2 || true
  exit 1
fi
# Probe well-known Windstille data paths (source tree layout under data/).
# Levels live under sectors/; there is no top-level levels/ directory.
for probe in fonts images sectors sounds controller music; do
  if [ ! -e "src/assets/$probe" ]; then
    echo "error: missing src/assets/$probe" >&2
    echo "       is GAME_DATA_DIR a full Windstille data/ tree?" >&2
    exit 1
  fi
done

cp "$KEYSTORE" debug.keystore

# Bake VERSION+g<rev> into WINDSTILLE_VERSION (see jni/Android.mk).
WINDSTILLE_VERSION="${WINDSTILLE_VERSION:-0.8.0-dev}"
echo "==> WINDSTILLE_VERSION=$WINDSTILLE_VERSION"

"$NDK/ndk-build" \
  NDK_PROJECT_PATH="$PWD/src" \
  APP_BUILD_SCRIPT="$PWD/src/jni/Android.mk" \
  NDK_APPLICATION_MK="$PWD/src/jni/Application.mk" \
  WINDSTILLE_VERSION="$WINDSTILLE_VERSION" \
  ENABLE_ANDROID_SOUND="${ENABLE_ANDROID_SOUND:-0}" \
  -j"${NIX_BUILD_CORES:-${JOBS:-$(nproc)}}"

mkdir -p out

# Package resources + manifest. Assets are added via zip below: old aapt's
# -A path has been unreliable with large trees in this pipeline.
"$BT/aapt" package -f \
  -M src/AndroidManifest.xml \
  -S src/res \
  -I "$PACKAGE_JAR" \
  -F out/base.apk

cp "$SDL_ANDROID_LIBS/dex/classes.dex" out/classes.dex
for abi in $TARGET_ABIS; do
  mkdir -p out/lib/"$abi"
  cp src/libs/"$abi"/*.so out/lib/"$abi"/
done

( cd out && "$BT/aapt" add base.apk classes.dex )
( cd out && zip -r base.apk lib )

# Inject assets/ into the APK (same layout AssetManager expects).
# Run from src/ so paths inside the zip are assets/...
( cd src && zip -r -9 ../out/base.apk assets )
echo "APK contents (assets sample):"
#unzip -l out/base.apk | grep -E 'assets/(images|levels)/' | head -20
#ASSET_IN_APK=$(unzip -l out/base.apk | grep -c ' assets/' || true)
#echo "Asset entries in APK: $ASSET_IN_APK"
#if [ "${ASSET_IN_APK:-0}" -lt 10 ]; then
#  echo "error: APK still has almost no assets after zip inject" >&2
#  exit 1
#fi

"$BT/zipalign" -f 4 out/base.apk out/aligned.apk

"$BT/apksigner" sign \
  --ks debug.keystore --ks-pass pass:android --key-pass pass:android \
  --out "out/$APP_NAME.apk" out/aligned.apk

echo "Final APK size: $(du -h "out/$APP_NAME.apk" | awk '{print $1}')"
"$BT/aapt" dump badging "out/$APP_NAME.apk"
