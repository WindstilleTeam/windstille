# Windstille - A Sci-Fi Action-Adventure Game
# Copyright (C) 2015 Ingo Ruhnke <grumbel@gmail.com>
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

cmake_minimum_required(VERSION 3.15)
project(wstdisplay VERSION 0.3.0)

include(mk/cmake/TinyCMMC.cmake)

option(BUILD_TESTS "Build test cases" OFF)
option(BUILD_EXTRA "Build extra stuff" OFF)

if (BUILD_TESTS)
  # add 'make test' target, use 'make test ARGS="-V"' or 'ctest -V' for verbose
  enable_testing()
endif(BUILD_TESTS)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

find_package(Threads REQUIRED)
find_package(Freetype REQUIRED)
find_package(OpenGL REQUIRED)
find_package(GLEW REQUIRED)
find_package(PkgConfig REQUIRED)

pkg_search_module(SDL2 REQUIRED sdl2 IMPORTED_TARGET)
pkg_search_module(SIGCXX REQUIRED sigc++-2.0 IMPORTED_TARGET)

# Build dependencies
function(build_dependencies)
  set(BUILD_TESTS OFF)
  set(BUILD_EXTRA OFF)

  tinycmmc_find_dependency(babyxml)
  tinycmmc_find_dependency(geom)
  tinycmmc_find_dependency(surf)
  tinycmmc_find_dependency(logmich)
endfunction()
build_dependencies()

# file(GLOB WSTDISPLAY_HEADERS RELATIVE ${CMAKE_CURRENT_SOURCE_DIR}
#   include/wstdisplay/*.hpp
#   include/wstdisplay/font/*.hpp
#   include/wstdisplay/scenegraph/*.hpp
#   include/wstsystem/*.hpp
#   )
file(GLOB WSTDISPLAY_SOURCES RELATIVE ${CMAKE_CURRENT_SOURCE_DIR}
  src/*.cpp
  src/font/*.?pp
  src/scenegraph/*.cpp
  )
add_library(wstdisplay STATIC ${WSTDISPLAY_SOURCES})
#set_target_properties(wstdisplay PROPERTIES
#  PUBLIC_HEADER "${WSTDISPLAY_HEADERS}")
target_include_directories(wstdisplay PRIVATE src/ include/wstdisplay include/wstsystem/)
target_include_directories(wstdisplay PUBLIC
  $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
  $<INSTALL_INTERFACE:include>
  )
target_compile_options(wstdisplay PRIVATE ${TINYCMMC_WARNINGS_CXX_FLAGS})
target_link_libraries(wstdisplay PUBLIC
  babyxml::babyxml
  geom::geom
  surf::surf
  logmich::logmich
  GLEW::GLEW
  OpenGL::GL
  OpenGL::GLU
  Freetype::Freetype
  PkgConfig::SDL2
  PkgConfig::SIGCXX
  Threads::Threads)

if(BUILD_TESTS)
  file(GLOB TEST_SOURCES RELATIVE ${CMAKE_CURRENT_SOURCE_DIR}
    test/*_test.cpp)
  foreach(SRC ${TEST_SOURCES})
    get_filename_component(SRC_BASENAME ${SRC} NAME_WE)
    add_executable(${SRC_BASENAME} ${SRC})
    target_link_libraries(${SRC_BASENAME} PRIVATE wstdisplay)
  endforeach(SRC)
endif()

if(BUILD_EXTRA)
  file(GLOB EXTRA_SOURCES RELATIVE ${CMAKE_CURRENT_SOURCE_DIR}
    extra/*.cpp)
  foreach(SRC ${EXTRA_SOURCES})
    get_filename_component(SRC_BASENAME ${SRC} NAME_WE)
    add_executable(${SRC_BASENAME} ${SRC})
    set_target_properties(${SRC_BASENAME} PROPERTIES OUTPUT_NAME "${PROJECT_NAME}-${SRC_BASENAME}")
    target_link_libraries(${SRC_BASENAME} PRIVATE wstdisplay)
    install(TARGETS ${SRC_BASENAME}
      RUNTIME DESTINATION ${CMAKE_INSTALL_FULL_BINDIR})
  endforeach(SRC)
endif()

# PUBLIC_HEADER can't handle subdirectories, so do this manually, try
# to use cmake's target_sources(...FILE_SET...) once 3.23 comes output
install(DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}/include/"
  DESTINATION "${CMAKE_INSTALL_INCLUDEDIR}/"
  FILES_MATCHING
  PATTERN "*.hpp"
  )

tinycmmc_export_and_install_library(wstdisplay)

# EOF #
