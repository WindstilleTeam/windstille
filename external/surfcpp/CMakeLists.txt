# surf - Software surface library
# Copyright (C) 2008-2020 Ingo Ruhnke <grumbel@gmail.com>
#
# This program is free software: you can redistribute it and/or modify
# it under the terms of the GNU General Public License as published by
# the Free Software Foundation, either version 3 of the License, or
# (at your option) any later version.
#
# This program is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
# GNU General Public License for more details.
#
# You should have received a copy of the GNU General Public License
# along with this program.  If not, see <http://www.gnu.org/licenses/>.

cmake_minimum_required(VERSION 3.10)

# Source of truth: top-level VERSION file (e.g. "0.2.0-dev"), or
# -DPROJECT_VERSION_FULL=... from the packaging (Nix flake appends
# .<revCount>+g<rev>).
if(NOT DEFINED PROJECT_VERSION_FULL)
  file(STRINGS "${CMAKE_CURRENT_SOURCE_DIR}/VERSION" PROJECT_VERSION_FULL LIMIT_COUNT 1)
endif()

# CMake project() VERSION expects major[.minor[.patch[.tweak]]]; strip
# any -dev / +git suffix for the numeric project version.
string(REGEX MATCH "^[0-9]+(\\.[0-9]+)*" _project_version_cmake "${PROJECT_VERSION_FULL}")

project(surf VERSION ${_project_version_cmake} LANGUAGES CXX)

include(GNUInstallDirs)
include(CMakePackageConfigHelpers)

option(WITH_MAGICKXX "Enable ImageMagick" OFF)
option(WITH_EXIF "Enable EXIF support" ON)
option(WITH_EXEC "Enable plugins needing exec() support" OFF)
option(BUILD_TESTS "Build test cases" OFF)
option(BUILD_BENCHMARKS "Build benchmarks" OFF)
option(BUILD_EXTRA "Build extra stuff" OFF)
option(WARNINGS "Enable extra compiler warnings" OFF)
option(WERROR "Treat warnings as errors" OFF)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# Compiler warnings (inlined from former tinycmmc)
set(SURF_WARNINGS_CXX_FLAGS "")
if(WERROR)
  list(APPEND SURF_WARNINGS_CXX_FLAGS -Werror)
endif()
if(WARNINGS)
  list(APPEND SURF_WARNINGS_CXX_FLAGS -Wall -Wextra)
  if(CMAKE_COMPILER_IS_GNUCXX)
    list(APPEND SURF_WARNINGS_CXX_FLAGS
      -Wcast-align -Wcast-qual -Wconversion -Wctor-dtor-privacy
      -Wdisabled-optimization -Wdouble-promotion -Weffc++ -Wformat=2
      -Winit-self -Winvalid-pch -Wlogical-op -Wmissing-format-attribute
      -Wmissing-noreturn -Wno-suggest-attribute=noreturn -Wno-unused-parameter
      -Wnon-virtual-dtor -Wold-style-cast -Woverloaded-virtual -Wpacked
      -Wredundant-decls -Wshadow -Wsign-promo -Wstrict-null-sentinel
      -Wsuggest-override -Wunreachable-code -Wzero-as-null-pointer-constant
      -pedantic)
    if(CMAKE_CXX_COMPILER_VERSION VERSION_GREATER 8)
      list(APPEND SURF_WARNINGS_CXX_FLAGS -Wint-in-bool-context)
    endif()
  elseif(CMAKE_CXX_COMPILER_ID MATCHES "Clang")
    list(APPEND SURF_WARNINGS_CXX_FLAGS
      -Weverything -Wno-c++98-compat -Wno-c++98-compat-pedantic
      -Wno-float-equal -Wno-padded -Wno-weak-vtables
      -Wno-disabled-macro-expansion -Wno-documentation -Wno-reserved-id-macro
      -Wno-sign-conversion -Wno-gnu-zero-variadic-macro-arguments
      -Wno-poison-system-directories -Wno-reserved-identifier
      -Wno-documentation-unknown-command
      -Wno-inconsistent-missing-destructor-override
      -Wno-deprecated-dynamic-exception-spec -Wno-deprecated
      -Wno-switch-enum -Wno-covered-switch-default
      -Wno-exit-time-destructors -Wno-global-constructors
      -Wno-duplicate-enum -Wno-unused-parameter -Wno-old-style-cast
      -Wno-unreachable-code-break -Wno-double-promotion
      -Wno-unused-private-field -Wno-unused-exception-parameter)
  endif()
endif()

find_package(PkgConfig REQUIRED)

# Default PNG/JPEG path is header-only stb_image (no libjpeg/libpng). System
# codecs remain optional for tools that need write support / EXIF pipelines.
option(SURF_USE_STB_IMAGE "Use stb_image for PNG/JPEG load (default; no system jpeg/png)" ON)
option(SURF_USE_SYSTEM_JPEG "Build libjpeg plugins (optional)" OFF)
option(SURF_USE_SYSTEM_PNG "Build libpng plugin (optional)" OFF)

if(SURF_USE_SYSTEM_JPEG)
  find_package(JPEG)
  if(NOT JPEG_FOUND)
    message(WARNING "SURF_USE_SYSTEM_JPEG=ON but JPEG not found; skipping")
  endif()
endif()
if(SURF_USE_SYSTEM_PNG)
  find_package(PNG)
  if(NOT PNG_FOUND)
    message(WARNING "SURF_USE_SYSTEM_PNG=ON but PNG not found; skipping")
  endif()
endif()

if(WITH_EXIF)
  pkg_search_module(EXIF libexif IMPORTED_TARGET)
endif()

if(WITH_MAGICKXX)
  pkg_search_module(MAGICKXX Magick++ GraphicsMagick++ IMPORTED_TARGET)
endif()

# already targets when built as a subdirectory next to them
if(NOT TARGET geom::geom)
  find_package(geom REQUIRED)
endif()
if(NOT TARGET logmich::logmich)
  find_package(logmich REQUIRED)
endif()

set(SURF_DEFINES)

file(GLOB SURF_SOURCES RELATIVE ${CMAKE_CURRENT_SOURCE_DIR}
  src/blend.cpp
  src/blend_scaled.cpp
  src/blendfunc.cpp
  src/blit.cpp
  src/channel.cpp
  src/color.cpp
  src/convert.cpp
  src/fill.cpp
  src/palette.cpp
  src/pixel_data.cpp
  src/pixel_format.cpp
  src/plugins/dds.cpp
  src/plugins/dds_surface.cpp
  src/plugins/exif.cpp
  src/plugins/imagemagick.cpp
  src/plugins/pnm.cpp
  src/save.cpp
  src/software_surface.cpp
  src/software_surface_factory.cpp
  src/transform.cpp
  src/util/filesystem.cpp
  )

file(GLOB SURF_EXEC_SOURCES RELATIVE ${CMAKE_CURRENT_SOURCE_DIR}
  src/plugins/kra.cpp
  src/plugins/rsvg.cpp
  src/plugins/ufraw.cpp
  src/plugins/vidthumb.cpp
  src/plugins/xcf.cpp
  src/util/exec.cpp
  )

if(WITH_EXEC)
  list(APPEND SURF_SOURCES ${SURF_EXEC_SOURCES})
  list(APPEND SURF_DEFINES "-DHAVE_EXEC")
endif()

# System libjpeg/libpng only when explicitly requested (and found).
if(SURF_USE_SYSTEM_JPEG AND JPEG_FOUND)
  list(APPEND SURF_SOURCES
    src/plugins/file_jpeg_compressor.cpp
    src/plugins/file_jpeg_decompressor.cpp
    src/plugins/jpeg.cpp
    src/plugins/jpeg_compressor.cpp
    src/plugins/jpeg_decompressor.cpp
    src/plugins/jpeg_memory_dest.cpp
    src/plugins/jpeg_memory_src.cpp
    src/plugins/mem_jpeg_compressor.cpp
    src/plugins/mem_jpeg_decompressor.cpp
  )
  list(APPEND SURF_DEFINES "-DHAVE_JPEG")
endif()
if(SURF_USE_SYSTEM_PNG AND PNG_FOUND)
  list(APPEND SURF_SOURCES src/plugins/png.cpp)
  list(APPEND SURF_DEFINES "-DHAVE_PNG")
endif()

if(SURF_USE_STB_IMAGE)
  list(APPEND SURF_SOURCES src/plugins/stb_image_loader.cpp)
  list(APPEND SURF_DEFINES "-DSURF_USE_STB_IMAGE=1")
  message(STATUS "surf: stb_image PNG/JPEG loader enabled (SURF_USE_STB_IMAGE)")
endif()

if(NOT SURF_USE_STB_IMAGE AND NOT (SURF_USE_SYSTEM_PNG AND PNG_FOUND) AND NOT (SURF_USE_SYSTEM_JPEG AND JPEG_FOUND))
  message(FATAL_ERROR "surf: enable SURF_USE_STB_IMAGE (default) or system JPEG/PNG plugins")
endif()

add_library(surf STATIC ${SURF_SOURCES})
set_target_properties(surf PROPERTIES
  CXX_STANDARD 20
  CXX_STANDARD_REQUIRED ON
  CXX_EXTENSIONS OFF
  )
target_compile_definitions(surf PRIVATE ${SURF_DEFINES})
target_compile_options(surf PRIVATE ${SURF_WARNINGS_CXX_FLAGS})
target_link_libraries(surf PUBLIC
  geom::geom
  logmich::logmich
  )
if(SURF_USE_SYSTEM_PNG AND PNG_FOUND)
  target_link_libraries(surf PUBLIC PNG::PNG)
endif()
if(SURF_USE_SYSTEM_JPEG AND JPEG_FOUND)
  target_link_libraries(surf PUBLIC JPEG::JPEG)
endif()
if(SURF_USE_STB_IMAGE)
  # Prefer -DSTB_IMAGE_INCLUDE_DIR=… (nix flake / r36s / android).
  if(DEFINED STB_IMAGE_INCLUDE_DIR AND EXISTS "${STB_IMAGE_INCLUDE_DIR}/stb_image.h")
    target_include_directories(surf PRIVATE "${STB_IMAGE_INCLUDE_DIR}")
    # saving PNG/JPEG without libpng/libjpeg
    if(EXISTS "${STB_IMAGE_INCLUDE_DIR}/stb_image_write.h")
      target_sources(surf PRIVATE src/plugins/stb_image_saver.cpp)
      target_compile_definitions(surf PRIVATE SURF_HAVE_STB_IMAGE_WRITE=1)
      message(STATUS "surf: stb_image_write PNG/JPEG saver enabled")
    endif()
  elseif(EXISTS "${CMAKE_CURRENT_SOURCE_DIR}/src/plugins/stb_image.h")
    # optional vendored copy next to the loader
  elseif(EXISTS "${CMAKE_SOURCE_DIR}/mk/android/app/jni/stb_image.h")
    target_include_directories(surf PRIVATE "${CMAKE_SOURCE_DIR}/mk/android/app/jni")
  else()
    message(FATAL_ERROR "surf: SURF_USE_STB_IMAGE requires stb_image.h (set STB_IMAGE_INCLUDE_DIR)")
  endif()
endif()
target_include_directories(surf PUBLIC
  $<INSTALL_INTERFACE:include>
  $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/src>
  $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
  $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include/surf>
  )

if(MAGICKXX_FOUND)
  target_link_libraries(surf PUBLIC PkgConfig::MAGICKXX)
  target_compile_definitions(surf PUBLIC "HAVE_MAGICKXX")
endif()

if(EXIF_FOUND)
  target_link_libraries(surf PUBLIC PkgConfig::EXIF)
  target_compile_definitions(surf PUBLIC "HAVE_EXIF")
endif()

if(BUILD_TESTS)
  find_package(GTest REQUIRED)
  pkg_search_module(SDL2 REQUIRED sdl2 IMPORTED_TARGET)

  file(GLOB TEST_SURF_SOURCES test/*_test.cpp)
  add_executable(test_surf ${TEST_SURF_SOURCES})
  set_target_properties(test_surf PROPERTIES
    CXX_STANDARD 20
    CXX_STANDARD_REQUIRED ON
    CXX_EXTENSIONS OFF)
  target_compile_options(test_surf PRIVATE ${SURF_WARNINGS_CXX_FLAGS})
  target_link_libraries(test_surf
    surf
    PkgConfig::SDL2
    GTest::GTest
    GTest::Main)

  enable_testing()
  add_test(NAME test_surf
    WORKING_DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}"
    COMMAND test_surf)
endif()

if(BUILD_BENCHMARKS)
  find_package(benchmark REQUIRED)

  file(GLOB BENCHMARK_SURF_SOURCES benchmark/*.cpp)
  add_executable(benchmark_surf ${BENCHMARK_SURF_SOURCES})
  set_target_properties(benchmark_surf PROPERTIES
    CXX_STANDARD 20
    CXX_STANDARD_REQUIRED ON
    CXX_EXTENSIONS OFF)
  target_compile_options(benchmark_surf PRIVATE ${SURF_WARNINGS_CXX_FLAGS})
  target_link_libraries(benchmark_surf
    surf
    benchmark::benchmark)
endif()

if(BUILD_EXTRA)
  file(GLOB EXTRA_SOURCES RELATIVE ${CMAKE_CURRENT_SOURCE_DIR}
    extra/*.cpp)
  foreach(SRC ${EXTRA_SOURCES})
    get_filename_component(SRC_BASENAME ${SRC} NAME_WE)
    add_executable(${SRC_BASENAME} ${SRC})
    set_target_properties(${SRC_BASENAME} PROPERTIES
      CXX_STANDARD 20
      CXX_STANDARD_REQUIRED ON
      CXX_EXTENSIONS OFF)
    target_compile_options(${SRC_BASENAME} PRIVATE ${SURF_WARNINGS_CXX_FLAGS})
    target_link_libraries(${SRC_BASENAME} surf)
    install(TARGETS ${SRC_BASENAME}
      RUNTIME DESTINATION "${CMAKE_INSTALL_BINDIR}")
  endforeach(SRC)
endif()

add_library(surf::surf ALIAS surf)

# Skip install/export when used as a subdirectory, e.g. by the ports
if(CMAKE_SOURCE_DIR STREQUAL CMAKE_CURRENT_SOURCE_DIR)
  install(DIRECTORY include/. DESTINATION "${CMAKE_INSTALL_INCLUDEDIR}")

  # Substituted into surf-config.cmake.in so consumers only pull JPEG/PNG when
  # this build actually linked them (stb_image builds leave these empty).
  if(SURF_USE_SYSTEM_JPEG AND JPEG_FOUND)
    set(SURF_CONFIG_FIND_JPEG "find_dependency(JPEG REQUIRED)")
  else()
    set(SURF_CONFIG_FIND_JPEG "")
  endif()
  if(SURF_USE_SYSTEM_PNG AND PNG_FOUND)
    set(SURF_CONFIG_FIND_PNG "find_dependency(PNG REQUIRED)")
  else()
    set(SURF_CONFIG_FIND_PNG "")
  endif()

  # Export and install library (inlined from former tinycmmc)
  install(TARGETS surf
    EXPORT surf
    ARCHIVE DESTINATION ${CMAKE_INSTALL_LIBDIR}
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
    PUBLIC_HEADER DESTINATION "${CMAKE_INSTALL_INCLUDEDIR}/surf")


  configure_package_config_file(
    "${CMAKE_CURRENT_SOURCE_DIR}/surf-config.cmake.in"
    "${CMAKE_CURRENT_BINARY_DIR}/surf-config.cmake"
    INSTALL_DESTINATION "${CMAKE_INSTALL_LIBDIR}/cmake/surf")
  write_basic_package_version_file(
    "${CMAKE_CURRENT_BINARY_DIR}/surf-config-version.cmake"
    VERSION "${PROJECT_VERSION}"
    COMPATIBILITY SameMinorVersion)
  export(EXPORT surf
    NAMESPACE surf::
    FILE "${CMAKE_CURRENT_BINARY_DIR}/surf-targets.cmake")
  install(EXPORT surf
    FILE surf-targets.cmake
    NAMESPACE surf::
    DESTINATION "${CMAKE_INSTALL_LIBDIR}/cmake/surf")
  install(FILES
    "${CMAKE_CURRENT_BINARY_DIR}/surf-config-version.cmake"
    "${CMAKE_CURRENT_BINARY_DIR}/surf-config.cmake"
    DESTINATION "${CMAKE_INSTALL_LIBDIR}/cmake/surf")
endif()

# EOF #
