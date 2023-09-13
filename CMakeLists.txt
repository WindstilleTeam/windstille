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

include(mk/cmake/TinyCMMC.cmake)

include(GetProjectVersion)
include(CheckIncludeFile)
include(InstallWithWrapper)

option(BUILD_EDITOR "Build Windstille editor" ON)
option(BUILD_TESTS "Build test cases" OFF)
option(BUILD_EXTRA "Build extra stuff" OFF)

if (BUILD_TESTS)
  # add 'make test' target, use 'make test ARGS="-V"' or 'ctest -V' for verbose
  enable_testing()
endif(BUILD_TESTS)

find_program(XCF2PNG xcf2png REQUIRED)
find_package(Threads REQUIRED)
find_package(JPEG REQUIRED)
find_package(PNG REQUIRED)
find_package(OpenGL REQUIRED)
find_package(GLEW REQUIRED)
find_package(glm REQUIRED)
find_package(fmt REQUIRED)
find_package(PkgConfig REQUIRED)
find_package(Squirrel REQUIRED)

pkg_search_module(SDL2 REQUIRED sdl2 IMPORTED_TARGET)

pkg_search_module(SIGCXX REQUIRED sigc++-2.0 IMPORTED_TARGET)

if(BUILD_EDITOR)
  pkg_search_module(GTKMM REQUIRED gtkmm-3.0 IMPORTED_TARGET)
endif()

# Build dependencies
function(build_dependencies)
  set(BUILD_TESTS OFF)
  set(BUILD_EXTRA OFF)

  tinycmmc_find_dependency(argpp)
  tinycmmc_find_dependency(babyxml)
  tinycmmc_find_dependency(geom)
  tinycmmc_find_dependency(logmich)
  tinycmmc_find_dependency(biio)
  tinycmmc_find_dependency(prio)
  tinycmmc_find_dependency(surf)
  tinycmmc_find_dependency(wstdisplay)
  tinycmmc_find_dependency(wstinput)
  tinycmmc_find_dependency(wstsound)
  tinycmmc_find_dependency(wstgui)

  find_program(MINISWIG miniswig)
  if(NOT MINISWIG)
    add_subdirectory(external/miniswig/ EXCLUDE_FROM_ALL)
  endif()
endfunction()
build_dependencies()

# compile scripting stuff
file(MAKE_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}/src/squirrel/)

set(SCRIPTING_SOURCE
  ${CMAKE_CURRENT_SOURCE_DIR}/src/scripting/interface.hpp
  ${CMAKE_CURRENT_SOURCE_DIR}/src/scripting/game_objects.hpp)
set(MINISWIG_TMP ${CMAKE_CURRENT_BINARY_DIR}/src/squirrel/miniswig.tmp)
set(WRAPPER_INTERFACE_HPP ${CMAKE_CURRENT_SOURCE_DIR}/src/scripting/wrapper.interface.hpp)

set(WRAPPER_CPP ${CMAKE_CURRENT_BINARY_DIR}/src/squirrel/wrapper.cpp)
set(WRAPPER_HPP ${CMAKE_CURRENT_BINARY_DIR}/src/squirrel/wrapper.hpp)

add_custom_target(GenerateWrapper
  BYPRODUCTS ${MINISWIG_TMP} ${WRAPPER_CPP} ${WRAPPER_HPP}
  DEPENDS
  ${SCRIPTING_SOURCE}
  ${WRAPPER_INTERFACE_HPP}
  ${MINISWIG}

  COMMAND ${CMAKE_CXX_COMPILER}
  -E
  -I${CMAKE_CURRENT_SOURCE_DIR}/src
  -I${CMAKE_CURRENT_BINARY_DIR}/src
  -x c
  -CC ${WRAPPER_INTERFACE_HPP}
  -o ${MINISWIG_TMP}
  -DSCRIPTING_API

  COMMAND ${MINISWIG}
  --input ${MINISWIG_TMP}
  --output-cpp ${WRAPPER_CPP}
  --output-hpp ${WRAPPER_HPP}
  --module windstille
  --select-namespace "Scripting"
  )

# compile wstlib
file(GLOB WSTLIB_SOURCES RELATIVE ${CMAKE_CURRENT_SOURCE_DIR}
  src/scenegraph/*.cpp
  src/font/*.cpp
  src/math/*.cpp
  src/navigation/*.cpp
  src/particles/*.cpp
  src/scenegraph/*.cpp
  src/sprite2d/*.cpp
  src/sprite3d/*.cpp
  src/system/*.cpp
  src/util/*.cpp)

# file(GLOB) won't add non-existing files, so add it here
list(APPEND WSTLIB_SOURCES ${WRAPPER_CPP})

add_library(wstlib STATIC ${WSTLIB_SOURCES})
add_dependencies(wstlib GenerateWrapper)
target_compile_options(wstlib PUBLIC ${TINYCMMC_WARNINGS_CXX_FLAGS})
target_compile_definitions(wstlib PUBLIC -DGLM_ENABLE_EXPERIMENTAL)
target_include_directories(wstlib PUBLIC
  src/
  ${CMAKE_CURRENT_BINARY_DIR}/src/)
target_link_libraries(wstlib PUBLIC
  biio::biio
  prio::prio
  wstdisplay
  wstinput::wstinput
  wstsound::wstsound
  wstgui::wstgui
  argpp::argpp
  babyxml::babyxml
  geom::geom
  logmich::logmich
  surf::surf
  Squirrel::Squirrel
  GLEW::GLEW
  JPEG::JPEG
  OpenGL::GL
  OpenGL::GLU
  PNG::PNG
  fmt::fmt
  PkgConfig::SDL2
  PkgConfig::SIGCXX
  Threads::Threads)

# compile galapix.sdl
if(BUILD_EDITOR)
  file(GLOB WINDSTILLE_EDITOR_SOURCES RELATIVE ${CMAKE_CURRENT_SOURCE_DIR}
    src/editor/*.cpp)

  add_executable(windstille-editor ${WINDSTILLE_EDITOR_SOURCES})
  target_compile_options(windstille-editor PUBLIC ${TINYCMMC_WARNINGS_CXX_FLAGS})
  target_link_libraries(windstille-editor
    wstlib
    PkgConfig::GTKMM)

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
  src/objects/*.cpp
  src/properties/*.cpp
  src/screen/*.cpp
  src/scripting/*.cpp
  src/squirrel/*.cpp
  src/tile/*.cpp)
add_executable(windstille ${WINDSTILLE_SOURCES})
target_compile_options(windstille PRIVATE ${TINYCMMC_WARNINGS_CXX_FLAGS})
target_link_libraries(windstille wstlib)
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

# EOF #
