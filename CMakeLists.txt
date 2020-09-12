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
project(windstille VERSION 0.3.0)

list(APPEND CMAKE_MODULE_PATH ${CMAKE_CURRENT_LIST_DIR}/external/cmake-modules/)

include(GetProjectVersion)
include(GNUInstallDirs)
include(CheckIncludeFile)
include(ClangTidy)
include(MaximumWarnings)
include(InstallWithWrapper)

option(BUILD_EDITOR "Build Windstille editor" ON)
option(BUILD_TESTS "Build test cases" OFF)
option(BUILD_EXTRA "Build extra stuff" OFF)

# list(REMOVE_ITEM WARNINGS_CXX_FLAGS )

if (BUILD_TESTS)
  # add 'make test' target, use 'make test ARGS="-V"' or 'ctest -V' for verbose
  enable_testing()
endif(BUILD_TESTS)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

find_program(XCF2PNG xcf2png)
if(NOT XCF2PNG)
  message(FATAL_ERROR "xcf2png not found")
endif()

find_package(Threads REQUIRED)
find_package(JPEG REQUIRED)
find_package(PNG REQUIRED)
find_package(Freetype REQUIRED)
find_package(BISON REQUIRED)
find_package(FLEX REQUIRED)
find_package(OpenAL REQUIRED)
find_package(OpenGL REQUIRED)
find_package(GLEW REQUIRED)
find_package(glm REQUIRED)
find_package(fmt REQUIRED)
find_package(PkgConfig REQUIRED)

find_library(CWIID_LIBRARY cwiid)

pkg_search_module(SDL2 REQUIRED sdl2 IMPORTED_TARGET)
pkg_search_module(SDL2_IMAGE REQUIRED SDL2_image IMPORTED_TARGET)

pkg_search_module(OGG REQUIRED ogg IMPORTED_TARGET)
pkg_search_module(VORBIS REQUIRED vorbis IMPORTED_TARGET)
pkg_search_module(VORBISFILE REQUIRED vorbisfile IMPORTED_TARGET)
pkg_search_module(SIGCXX REQUIRED sigc++-2.0 IMPORTED_TARGET)

pkg_search_module(SQUIRREL squirrel3 IMPORTED_TARGET)
if(NOT SQUIRREL_FOUND)
  find_library(SQUIRREL_LIBRARY NAMES "squirrel" "squirrel3" REQUIRED)
  find_library(SQSTDLIB_LIBRARY NAMES "sqstdlib" "sqstdlib3" REQUIRED)
  find_path(SQUIRREL_INCLUDE_DIRS NAMES "squirrel.h" PATH_SUFFIXES "/" "squirrel/" "squirrel3/")
  if (NOT SQUIRREL_INCLUDE_DIRS)
    message(FATAL_ERROR "squirrel not found")
  endif()

  add_library(PkgConfig::SQUIRREL INTERFACE IMPORTED)
  target_link_libraries(PkgConfig::SQUIRREL INTERFACE ${SQUIRREL_LIBRARY} ${SQSTDLIB_LIBRARY})
  target_include_directories(PkgConfig::SQUIRREL INTERFACE ${SQUIRREL_INCLUDE_DIRS})
  message(STATUS "Found Squirrel: ${SQUIRREL_LIBRARY} ${SQSTDLIB_LIBRARY} ${SQUIRREL_INCLUDE_DIRS}")
endif()

if(BUILD_EDITOR)
  pkg_search_module(GTKMM REQUIRED gtkmm-2.4 IMPORTED_TARGET)
  pkg_search_module(GTKGLEXTMM REQUIRED gtkglextmm-1.2 IMPORTED_TARGET)
endif()

add_subdirectory(external/argparser/ EXCLUDE_FROM_ALL)
add_subdirectory(external/babyxml/ EXCLUDE_FROM_ALL)
add_subdirectory(external/sexp-cpp/ EXCLUDE_FROM_ALL)

file(GLOB LOGMICH_SOURCES_CXX RELATIVE ${CMAKE_CURRENT_SOURCE_DIR}
  external/logmich/src/*.cpp)
add_library(logmich STATIC ${LOGMICH_SOURCES_CXX})
target_include_directories(logmich SYSTEM PUBLIC external/logmich/include/)
set_target_properties(logmich PROPERTIES CXX_CLANG_TIDY "")

include_directories(.)
include_directories(src/)
include_directories(${CMAKE_CURRENT_BINARY_DIR}/src/)

bison_target(parser
  ${CMAKE_CURRENT_SOURCE_DIR}/external/miniswig/parser.yy
  ${CMAKE_CURRENT_BINARY_DIR}/parser.cpp
  COMPILE_FLAGS "--no-lines -d")
flex_target(MyScanner
  ${CMAKE_CURRENT_SOURCE_DIR}/external/miniswig/lexer.ll
  ${CMAKE_CURRENT_BINARY_DIR}/lexer.cpp)

file(GLOB MINISWIG_SOURCES RELATIVE ${CMAKE_CURRENT_SOURCE_DIR}
  external/miniswig/create_docu.cpp
  external/miniswig/xmlwriter.cpp
  external/miniswig/create_wrapper.cpp
  external/miniswig/main.cpp
  external/miniswig/tree.cpp)
list(APPEND MINISWIG_SOURCES
  ${CMAKE_CURRENT_BINARY_DIR}/lexer.cpp
  ${CMAKE_CURRENT_BINARY_DIR}/parser.cpp)
add_executable(miniswig ${MINISWIG_SOURCES})
target_include_directories(miniswig SYSTEM PRIVATE external/miniswig/)
set_target_properties(miniswig PROPERTIES CXX_CLANG_TIDY "")

# compile scripting stuff
file(MAKE_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}/src/scripting/)
set(SCRIPTING_SOURCE
  ${CMAKE_CURRENT_SOURCE_DIR}/src/scripting/interface.hpp
  ${CMAKE_CURRENT_SOURCE_DIR}/src/scripting/game_objects.hpp)
set(TARGET ${CMAKE_CURRENT_BINARY_DIR}/src/scripting/miniswig.tmp)
set(SOURCE ${CMAKE_CURRENT_SOURCE_DIR}/src/scripting/wrapper.interface.hpp)
add_custom_command(
  OUTPUT ${TARGET}
  COMMAND ${CMAKE_CXX_COMPILER}
  ARGS
  -E
  -I${CMAKE_CURRENT_SOURCE_DIR}/src
  -I${CMAKE_CURRENT_BINARY_DIR}/src
  -x c
  -CC ${SOURCE}
  -o ${TARGET}
  -DSCRIPTING_API
  DEPENDS
  ${SCRIPTING_SOURCE}
  ${SOURCE})

set(TARGET_CPP ${CMAKE_CURRENT_BINARY_DIR}/src/scripting/wrapper.cpp)
set(TARGET_HPP ${CMAKE_CURRENT_BINARY_DIR}/src/scripting/wrapper.hpp)
set(SOURCE ${CMAKE_CURRENT_BINARY_DIR}/src/scripting/miniswig.tmp)
add_custom_command(
  OUTPUT ${TARGET_CPP} ${TARGET_HPP}
  COMMAND ${CMAKE_CURRENT_BINARY_DIR}/miniswig
  --input ${SOURCE}
  --output-cpp ${TARGET_CPP}
  --output-hpp ${TARGET_HPP}
  --module windstille
  --select-namespace "Scripting"
  DEPENDS
  ${CMAKE_CURRENT_BINARY_DIR}/miniswig
  ${SOURCE})

# compile libgalapix
file(GLOB WSTLIB_SOURCES RELATIVE ${CMAKE_CURRENT_SOURCE_DIR}
  src/display/*.cpp
  src/font/*.cpp
  src/lisp/*.cpp
  src/math/*.cpp
  src/navigation/*.cpp
  src/particles/*.cpp
  src/scenegraph/*.cpp
  src/sound/*.cpp
  src/sprite2d/*.cpp
  src/sprite3d/*.cpp
  src/system/*.cpp
  src/util/*.cpp)
add_library(wstlib STATIC ${WSTLIB_SOURCES})
target_compile_options(wstlib PUBLIC ${WARNINGS_CXX_FLAGS})
target_include_directories(wstlib SYSTEM PUBLIC
  ${OPENAL_INCLUDE_DIR})
target_link_libraries(wstlib PUBLIC
  argparser
  babyxml
  logmich
  sexp
  ${OPENAL_LIBRARY}
  PkgConfig::SQUIRREL
  Freetype::Freetype
  GLEW::GLEW
  JPEG::JPEG
  OpenGL::GL
  OpenGL::GLU
  PNG::PNG
  PkgConfig::OGG
  PkgConfig::SDL2
  PkgConfig::SDL2_IMAGE
  PkgConfig::SIGCXX
  PkgConfig::VORBIS
  PkgConfig::VORBISFILE
  Threads::Threads
  fmt::fmt)

# compile galapix.sdl
if(BUILD_EDITOR)
  file(GLOB WINDSTILLE_EDITOR_SOURCES RELATIVE ${CMAKE_CURRENT_SOURCE_DIR}
    src/editor/*.cpp)

  add_executable(windstille-editor ${WINDSTILLE_EDITOR_SOURCES})
  target_compile_options(windstille-editor PUBLIC ${WARNINGS_CXX_FLAGS})
  target_link_libraries(windstille-editor
    wstlib
    PkgConfig::GTKMM
    PkgConfig::GTKGLEXTMM)

  install_with_wrapper(TARGETS windstille-editor)

  install(FILES
    windstille-editor.svg
    DESTINATION ${CMAKE_INSTALL_DATADIR}/icons/hicolor/scalable/apps)

  install(FILES
    windstille-editor.desktop
    DESTINATION ${CMAKE_INSTALL_DATADIR}/applications)
endif()

file(GLOB WINDSTILLE_SOURCES RELATIVE ${CMAKE_CURRENT_SOURCE_DIR}
  src/app/*.cpp
  src/armature/*.cpp
  src/collision/*.cpp
  src/engine/*.cpp
  src/gui/*.cpp
  src/hud/*.cpp
  src/input/*.cpp
  src/objects/*.cpp
  src/properties/*.cpp
  src/screen/*.cpp
  src/scripting/*.cpp
  src/tile/*.cpp)
add_executable(windstille ${WINDSTILLE_SOURCES})
target_compile_options(windstille PRIVATE ${WARNINGS_CXX_FLAGS})
target_link_libraries(windstille wstlib)

if(CWIID_LIBRARY)
  target_compile_options(windstille PUBLIC -DHAVE_CWIID)
  target_link_libraries(windstille ${CWIID_LIBRARY})
endif()

install_with_wrapper(TARGETS windstille)

# build and install data
file(MAKE_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}/data/images/decal/)

# convert .xcf to .png
file(GLOB DECAL_SOURCES RELATIVE ${CMAKE_CURRENT_SOURCE_DIR}
  data/images/decal/*.xcf)

foreach(SOURCE ${DECAL_SOURCES})
  string(REGEX REPLACE "\\.xcf$" ".png" TARGET ${SOURCE})

  set(SOURCE "${CMAKE_CURRENT_SOURCE_DIR}/${SOURCE}")
  set(TARGET "${CMAKE_CURRENT_BINARY_DIR}/${TARGET}")

  add_custom_command(
    OUTPUT ${TARGET}
    COMMAND ${XCF2PNG}
    ARGS
    ${SOURCE}
    -o ${TARGET}
    DEPENDS ${SOURCE})

  list(APPEND WINDSTILLE_GENERATED_DATA_FILES ${TARGET})
endforeach(SOURCE)

add_custom_target(windstille-data ALL
  DEPENDS ${WINDSTILLE_GENERATED_DATA_FILES})

file(GLOB WINDSTILLE_DATA_FILES RELATIVE ${CMAKE_CURRENT_SOURCE_DIR}
  data/*.scm
  data/controller/*.scm
  data/scripts/*.nut
  data/sounds/*.wav
  data/sounds/*.ogg
  data/fonts/*.ttf
  data/images/*.png
  data/images/*/*.png
  data/images/*/*/*.png)

foreach(SOURCE ${WINDSTILLE_DATA_FILES})
  get_filename_component(DIR ${SOURCE} DIRECTORY)
  file(COPY ${SOURCE}
    DESTINATION ${CMAKE_CURRENT_BINARY_DIR}/${DIR})
endforeach(SOURCE)

# install windstille data
install(DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/data/
  DESTINATION ${CMAKE_INSTALL_DATADIR}/${PROJECT_NAME})

if(BUILD_TESTS)
  file(GLOB TEST_SOURCES RELATIVE ${CMAKE_CURRENT_SOURCE_DIR}
    test/*_test.cpp)
  foreach(SRC ${TEST_SOURCES})
    get_filename_component(SRC_BASENAME ${SRC} NAME_WE)
    add_executable(${SRC_BASENAME} ${SRC})
    target_link_libraries(${SRC_BASENAME} wstlib)
  endforeach(SRC)
endif()

if(BUILD_EXTRA)
  file(GLOB SLIDESHOW_SOURCE RELATIVE ${CMAKE_CURRENT_SOURCE_DIR}
    extra/slideshow/*.cpp
    extra/slideshow/plugins/*.cpp)
  add_executable(slideshow ${SLIDESHOW_SOURCE})
  target_link_libraries(slideshow wstlib)
  target_include_directories(slideshow PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/extra/)

  file(GLOB SHADERTEST_SOURCE RELATIVE ${CMAKE_CURRENT_SOURCE_DIR}
    extra/shadertest/*.cpp)
  add_executable(shadertest ${SHADERTEST_SOURCE})
  target_link_libraries(shadertest wstlib)

  file(GLOB LENSFLARE_SOURCE RELATIVE ${CMAKE_CURRENT_SOURCE_DIR}
    extra/lensflare/*.cpp)
  add_executable(lensflare ${LENSFLARE_SOURCE})
  target_link_libraries(lensflare wstlib)

  file(GLOB MEMLEAK_SOURCE RELATIVE ${CMAKE_CURRENT_SOURCE_DIR}
    extra/memleak/*.cpp)
  add_executable(memleak ${MEMLEAK_SOURCE})
  target_link_libraries(memleak wstlib)

  file(GLOB 2DSHADOW_SOURCE RELATIVE ${CMAKE_CURRENT_SOURCE_DIR}
    extra/2dshadow/*.cpp)
  add_executable(2dshadow ${2DSHADOW_SOURCE})
  target_link_libraries(2dshadow wstlib)

  file(GLOB EXTRA_SOURCES RELATIVE ${CMAKE_CURRENT_SOURCE_DIR}
    extra/*.cpp)
  foreach(SRC ${EXTRA_SOURCES})
    get_filename_component(SRC_BASENAME ${SRC} NAME_WE)
    add_executable(${SRC_BASENAME} ${SRC})
    target_link_libraries(${SRC_BASENAME} wstlib)
  endforeach(SRC)
endif()

install(FILES
  windstille.svg
  DESTINATION ${CMAKE_INSTALL_DATADIR}/icons/hicolor/scalable/apps)

install(FILES
  windstille.desktop
  DESTINATION ${CMAKE_INSTALL_DATADIR}/applications)

configure_file(windstille.appdata.xml.in ${CMAKE_CURRENT_BINARY_DIR}/windstille.appdata.xml)
install(FILES ${CMAKE_CURRENT_BINARY_DIR}/windstille.appdata.xml
  DESTINATION ${CMAKE_INSTALL_DATADIR}/metainfo)

if (BUILD_TESTS)
  add_test(NAME windstille.appdata.xml
    WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}
    COMMAND appstream-util validate-relax ${CMAKE_CURRENT_BINARY_DIR}/windstille.appdata.xml)
endif(BUILD_TESTS)

# EOF #