LOCAL_PATH := $(call my-dir)

# ---------------------------------------------------------------------------
# Prebuilt OpenAL Soft + libmodplug (per-ABI static libs from AUDIO_ANDROID_LIBS)
# ---------------------------------------------------------------------------
ifeq ($(ENABLE_ANDROID_SOUND),1)

include $(CLEAR_VARS)
LOCAL_MODULE := openal
LOCAL_SRC_FILES := ../audio/$(TARGET_ARCH_ABI)/lib/libopenal.a
LOCAL_EXPORT_C_INCLUDES := $(LOCAL_PATH)/../audio/include $(LOCAL_PATH)/../audio/include/AL
include $(PREBUILT_STATIC_LIBRARY)

include $(CLEAR_VARS)
LOCAL_MODULE := modplug
LOCAL_SRC_FILES := ../audio/$(TARGET_ARCH_ABI)/lib/libmodplug.a
LOCAL_EXPORT_C_INCLUDES := $(LOCAL_PATH)/../audio/include
include $(PREBUILT_STATIC_LIBRARY)

endif


# ---------------------------------------------------------------------------
# Optional prebuilt FreeType (jni/freetype/<abi>/lib/libfreetype.a)
# ---------------------------------------------------------------------------
ifneq ($(wildcard $(LOCAL_PATH)/../freetype/$(TARGET_ARCH_ABI)/lib/libfreetype.a),)
include $(CLEAR_VARS)
LOCAL_MODULE := freetype
LOCAL_SRC_FILES := ../freetype/$(TARGET_ARCH_ABI)/lib/libfreetype.a
LOCAL_EXPORT_C_INCLUDES := $(LOCAL_PATH)/../freetype/include
include $(PREBUILT_STATIC_LIBRARY)
endif

# ---------------------------------------------------------------------------
# libmain — Windstille + staged external sources
# ---------------------------------------------------------------------------
include $(CLEAR_VARS)

LOCAL_MODULE := main

# Recursively collect C++ under this tree (game + deps/*).
RWILDCARD = $(foreach d,$(wildcard $1*),$(call RWILDCARD,$d/,$2) $(filter $(subst *,%,$2),$d))
LOCAL_SRC_FILES := $(patsubst $(LOCAL_PATH)/%,%,$(call RWILDCARD,$(LOCAL_PATH)/,%.cpp))
LOCAL_SRC_FILES += $(patsubst $(LOCAL_PATH)/%,%,$(wildcard $(LOCAL_PATH)/*.c))

# Exclude desktop-only / optional backends when present under the tree.
LOCAL_SRC_FILES := $(filter-out %/win32/% win32/%,$(LOCAL_SRC_FILES))
# Editor is GTK desktop-only — never ship it in the APK.
LOCAL_SRC_FILES := $(filter-out %/editor/% editor/%,$(LOCAL_SRC_FILES))
LOCAL_SRC_FILES := $(filter-out %/json_reader_impl.cpp %/json_writer_impl.cpp %/jsonpretty_writer_impl.cpp,$(LOCAL_SRC_FILES))
# Prefer deps/prio over the priocpp alias (same sources twice → duplicate symbols / double work).
LOCAL_SRC_FILES := $(filter-out %/deps/priocpp/%,$(LOCAL_SRC_FILES))
# No system libjpeg/libpng on Android NDK tree yet; drop surfcpp image plugins
# that need them (DDS/PNM remain). TODO: stage libjpeg-turbo + libpng statics
# like audio libs and re-enable JPEG/PNG SoftwareSurface loaders.
# Note: GNU make filter-out allows only one % per pattern.
LOCAL_SRC_FILES := $(filter-out \
  %/plugins/jpeg.cpp \
  %/plugins/jpeg_compressor.cpp \
  %/plugins/jpeg_decompressor.cpp \
  %/plugins/jpeg_memory_dest.cpp \
  %/plugins/jpeg_memory_src.cpp \
  %/plugins/file_jpeg_compressor.cpp \
  %/plugins/file_jpeg_decompressor.cpp \
  %/plugins/mem_jpeg_compressor.cpp \
  %/plugins/mem_jpeg_decompressor.cpp \
  %/plugins/png.cpp,\
  $(LOCAL_SRC_FILES))

ifneq ($(ENABLE_ANDROID_SOUND),1)
LOCAL_SRC_FILES := $(filter-out %/sound_real.cpp,$(LOCAL_SRC_FILES))
# Without OpenAL, drop wstsound sources staged under deps/
LOCAL_SRC_FILES := $(filter-out %/deps/wstsound/%,$(LOCAL_SRC_FILES))
endif

LOCAL_C_INCLUDES := \
	$(LOCAL_PATH)/../SDL/include \
	$(LOCAL_PATH)/../SDL/include/SDL2 \
	$(LOCAL_PATH) \
	$(LOCAL_PATH)/../external_includes \
	$(LOCAL_PATH)/../external_includes/argpp \
	$(LOCAL_PATH)/../external_includes/geom \
	$(LOCAL_PATH)/../external_includes/logmich \
	$(LOCAL_PATH)/../external_includes/prio \
	$(LOCAL_PATH)/../external_includes/strut \
	$(LOCAL_PATH)/../external_includes/sexp \
	$(LOCAL_PATH)/../external_includes/surf \
	$(LOCAL_PATH)/../external_includes/wstdisplay \
	$(LOCAL_PATH)/../external_includes/wstinput \
	$(LOCAL_PATH)/../external_includes/wstgui \
	$(LOCAL_PATH)/../external_includes/wstsound \
	$(LOCAL_PATH)/../external_includes/babyxml \
	$(LOCAL_PATH)/../external_includes/biio \
	$(LOCAL_PATH)/../external_includes/squirrel \
	$(LOCAL_PATH)/deps/argpp \
	$(LOCAL_PATH)/deps/logmich \
	$(LOCAL_PATH)/deps/sexpcpp \
	$(LOCAL_PATH)/deps/strutcpp \
	$(LOCAL_PATH)/deps/priocpp \
	$(LOCAL_PATH)/deps/babyxml \
	$(LOCAL_PATH)/deps/biiocpp \
	$(LOCAL_PATH)/deps/surfcpp \
	$(LOCAL_PATH)/deps/wstdisplay \
	$(LOCAL_PATH)/deps/wstinput \
	$(LOCAL_PATH)/deps/wstgui \
	$(LOCAL_PATH)/deps/tinygettext \
	$(LOCAL_PATH)/deps/squirrel \
	$(LOCAL_PATH)/deps/wstsound

ifeq ($(ENABLE_ANDROID_SOUND),1)
# libmodplug: <libmodplug/modplug.h>  OpenAL Soft install: include/AL/*.h
# Non-Emscripten wstsound uses <al.h>/<alc.h>/<alext.h> (not <AL/al.h>).
LOCAL_C_INCLUDES += $(LOCAL_PATH)/../audio/include
LOCAL_C_INCLUDES += $(LOCAL_PATH)/../audio/include/AL
endif

ifneq ($(wildcard $(LOCAL_PATH)/../freetype/include/ft2build.h),)
LOCAL_C_INCLUDES += $(LOCAL_PATH)/../freetype/include $(LOCAL_PATH)/../freetype/include/freetype2
endif
ifneq ($(wildcard $(LOCAL_PATH)/../freetype/$(TARGET_ARCH_ABI)/lib/libfreetype.a),)
LOCAL_STATIC_LIBRARIES += freetype
endif

LOCAL_SHARED_LIBRARIES := SDL2

LOCAL_LDLIBS := -llog -landroid -lz -lGLESv2 -lEGL

ifeq ($(ENABLE_ANDROID_SOUND),1)
# WHOLE_STATIC: OpenAL Soft registers backends via static constructors;
# regular static linking drops them and alcOpenDevice fails silently.
LOCAL_WHOLE_STATIC_LIBRARIES := openal
LOCAL_STATIC_LIBRARIES := modplug
LOCAL_LDLIBS += -lOpenSLES
LOCAL_CFLAGS += -DUSE_SDL2 -DANDROID -DWINDSTILLE_USE_GLES=1 -DWSTDISPLAY_USE_GLES=1 -DGLM_ENABLE_EXPERIMENTAL
LOCAL_CPPFLAGS += -DUSE_SDL2 -DANDROID -DWINDSTILLE_USE_GLES=1 -DWSTDISPLAY_USE_GLES=1 -DGLM_ENABLE_EXPERIMENTAL -std=c++23 -fexceptions -D_LIBCPP_ENABLE_EXPERIMENTAL=1
LOCAL_CFLAGS += -DWSTSOUND_WITH_MODPLUG=1
LOCAL_CPPFLAGS += -DWSTSOUND_WITH_MODPLUG=1
else
LOCAL_CFLAGS += -DUSE_SDL2 -DANDROID -DWINDSTILLE_NO_SOUND=1 -DWINDSTILLE_USE_GLES=1 -DWSTDISPLAY_USE_GLES=1 -DGLM_ENABLE_EXPERIMENTAL
LOCAL_CPPFLAGS += -DUSE_SDL2 -DANDROID -DWINDSTILLE_NO_SOUND=1 -DWINDSTILLE_USE_GLES=1 -DWSTDISPLAY_USE_GLES=1 -DGLM_ENABLE_EXPERIMENTAL -std=c++23 -fexceptions -D_LIBCPP_ENABLE_EXPERIMENTAL=1
endif

LOCAL_CFLAGS += -DPRIO_USE_SEXPCPP=1 -DTINYGETTEXT_UTF8_ONLY=1
LOCAL_CPPFLAGS += -DPRIO_USE_SEXPCPP=1 -DTINYGETTEXT_UTF8_ONLY=1
ifndef WINDSTILLE_VERSION
WINDSTILLE_VERSION := 0.8.0-dev
endif
LOCAL_CFLAGS += -DPROJECT_VERSION=\"$(WINDSTILLE_VERSION)\" -DPROJECT_NAME=\"windstille\"
LOCAL_CPPFLAGS += -DPROJECT_VERSION=\"$(WINDSTILLE_VERSION)\" -DPROJECT_NAME=\"windstille\"
LOCAL_CFLAGS += -DWINDSTILLE_DEFAULT_DATADIR=\"\" -DDATA_PREFIX=\"\"
LOCAL_CPPFLAGS += -DWINDSTILLE_DEFAULT_DATADIR=\"\" -DDATA_PREFIX=\"\"

LOCAL_CPP_FEATURES := exceptions rtti

include $(BUILD_SHARED_LIBRARY)
