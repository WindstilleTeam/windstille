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
project(surf VERSION 0.0.0)

include(mk/cmake/TinyCMMC.cmake)

option(WITH_MAGICKXX "Enable ImageMagick" OFF)
option(WITH_EXIF "Enable EXIF support" ON)
option(WITH_EXEC "Enable plugins needing exec() support" OFF)

find_package(PkgConfig REQUIRED)
find_package(JPEG REQUIRED)
find_package(PNG REQUIRED)

if(WITH_EXIF)
  pkg_search_module(EXIF libexif IMPORTED_TARGET)
endif()

if(WITH_MAGICKXX)
  # Not using find_package(), see:
  # https://bugs.debian.org/cgi-bin/bugreport.cgi?bug=776832
  # find_package(ImageMagick COMPONENTS Magick++ REQUIRED)
  pkg_search_module(MAGICKXX Magick++ GraphicsMagick++ IMPORTED_TARGET)
endif()

function(build_dependencies)
  set(BUILD_TESTS OFF)

  find_package(geom QUIET)
  if(NOT TARGET geom::geom)
    add_subdirectory(external/geomcpp/ EXCLUDE_FROM_ALL)
  endif()

  find_package(logmich QUIET)
  if(NOT TARGET logmich::logmich)
    add_subdirectory(external/logmich/ EXCLUDE_FROM_ALL)
  endif()
endfunction()
build_dependencies()

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
  src/plugins/file_jpeg_compressor.cpp
  src/plugins/file_jpeg_decompressor.cpp
  src/plugins/imagemagick.cpp
  src/plugins/jpeg.cpp
  src/plugins/jpeg_compressor.cpp
  src/plugins/jpeg_decompressor.cpp
  src/plugins/jpeg_memory_dest.cpp
  src/plugins/jpeg_memory_src.cpp
  src/plugins/mem_jpeg_compressor.cpp
  src/plugins/mem_jpeg_decompressor.cpp
  src/plugins/png.cpp
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
  list(append SURF_SOURCES ${SURF_EXEC_SOURCES})
  list(append SURF_DEFINES "-DHAVE_EXEC")
endif()

add_library(surf STATIC ${SURF_SOURCES})
set_target_properties(surf PROPERTIES
  CXX_STANDARD 20
  CXX_STANDARD_REQUIRED ON
  CXX_EXTENSIONS OFF
  )
target_compile_definitions(surf PRIVATE
  ${SURF_DEFINES}
  )
target_link_libraries(surf PUBLIC
  geom::geom
  logmich::logmich
  PNG::PNG
  JPEG::JPEG
  )
target_include_directories(surf PUBLIC
  $<INSTALL_INTERFACE:include>
  $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/src>
  $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
  $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include/surf>
  )

if(MAGICKXX_FOUND)
  target_link_libraries(surf PUBLIC
    PkgConfig::MAGICKXX)
  target_compile_definitions(surf PUBLIC "HAVE_MAGICKXX")
endif()

if(EXIF_FOUND)
  target_link_libraries(surf PUBLIC
    PkgConfig::EXIF)
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
  target_compile_options(test_surf PRIVATE ${TINYCMMC_MAXIMUM_WARNINGS_CXX_FLAGS})
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

  # build benchmarks
  file(GLOB BENCHMARK_SURF_SOURCES benchmark/*.cpp)
  add_executable(benchmark_surf ${BENCHMARK_SURF_SOURCES})
  set_target_properties(benchmark_surf PROPERTIES
    CXX_STANDARD 20
    CXX_STANDARD_REQUIRED ON
    CXX_EXTENSIONS OFF)
  target_compile_options(benchmark_surf PRIVATE ${TINYCMMC_MAXIMUM_WARNINGS_CXX_FLAGS})
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
    target_compile_options(${SRC_BASENAME} PRIVATE ${TINYCMMC_MAXIMUM_WARNINGS_CXX_FLAGS})
    target_link_libraries(${SRC_BASENAME} surf)
    install(TARGETS ${SRC_BASENAME}
      RUNTIME DESTINATION "${CMAKE_INSTALL_BINDIR}")
  endforeach(SRC)
endif()

install(DIRECTORY include/. DESTINATION "${CMAKE_INSTALL_INCLUDEDIR}")

tinycmmc_export_and_install_library(surf)

# EOF #
