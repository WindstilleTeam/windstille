# Windstille
# Copyright (C) 2020 Ingo Ruhnke <grumbel@gmail.com>
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

cmake_minimum_required(VERSION 3.13)
project(windstille-0.2 VERSION 0.2.0)

set(TINYCMMC_MODULE_PATH "${CMAKE_CURRENT_LIST_DIR}/external/tinycmmc/modules/")
find_package(tinycmmc CONFIG)
message(STATUS "tinycmmc module path: ${TINYCMMC_MODULE_PATH}")
list(APPEND CMAKE_MODULE_PATH ${TINYCMMC_MODULE_PATH})

option(BUILD_TESTS "Build test cases" OFF)
option(BUILD_EXTRA "Build extras" OFF)

include(GetProjectVersion)
include(GNUInstallDirs)
# include(ClangTidy)
# include(MaximumWarnings)
include(InstallWithWrapper)

cmake_policy(SET CMP0078 OLD) # Swig
cmake_policy(SET CMP0086 OLD) # Swig

find_package(SWIG REQUIRED)
include(${SWIG_USE_FILE})

find_package(OpenGL REQUIRED)
find_package(Threads REQUIRED)
find_package(PkgConfig REQUIRED)
pkg_search_module(GUILE REQUIRED guile-3.0 IMPORTED_TARGET)
pkg_search_module(CLANCORE REQUIRED clanCore-1.0 IMPORTED_TARGET)
pkg_search_module(CLANAPP REQUIRED clanApp-1.0 IMPORTED_TARGET)
pkg_search_module(CLANDISPLAY REQUIRED clanDisplay-1.0 IMPORTED_TARGET)
pkg_search_module(CLANGUI REQUIRED clanGUI-1.0 IMPORTED_TARGET)
pkg_search_module(CLANGUISTYLESILVER REQUIRED clanGUIStyleSilver-1.0 IMPORTED_TARGET)
pkg_search_module(CLANGL REQUIRED clanGL-1.0 IMPORTED_TARGET)
pkg_search_module(CLANSOUND REQUIRED clanSound-1.0 IMPORTED_TARGET)
pkg_search_module(CLANVORBIS REQUIRED clanVorbis-1.0 IMPORTED_TARGET)

set_source_files_properties(src/scripting/windstille.i PROPERTIES CPLUSPLUS ON)
swig_add_library(libwindstille_scripting TYPE STATIC LANGUAGE guile SOURCES src/scripting/windstille.i)
target_include_directories(libwindstille_scripting PUBLIC src/)
target_link_libraries(libwindstille_scripting PUBLIC PkgConfig::GUILE)

file(GLOB LIBWINDSTILLE_COLLISION_SOURCES
  src/collision/*.cxx)

add_library(libwindstille_collision STATIC ${LIBWINDSTILLE_COLLISION_SOURCES})
target_include_directories(libwindstille_collision PUBLIC src/)
target_link_libraries(libwindstille_collision PUBLIC
  PkgConfig::CLANCORE
  PkgConfig::CLANDISPLAY)

file(GLOB WINDSTILLE_SOURCES
  src/*.cxx
  src/editor/*.cxx
  src/guile/*.cxx
  src/guistyle/*.cxx
  src/input/*.cxx
  src/math/*.cxx
  src/scripting/*.cxx)

add_executable(${PROJECT_NAME} ${WINDSTILLE_SOURCES})
target_compile_options(${PROJECT_NAME} PRIVATE ${WARNINGS_CXX_FLAGS})
target_include_directories(${PROJECT_NAME} PUBLIC src/)
target_link_libraries(${PROJECT_NAME} PRIVATE
  libwindstille_collision
  libwindstille_scripting
  PkgConfig::GUILE
  PkgConfig::CLANCORE
  PkgConfig::CLANAPP
  PkgConfig::CLANDISPLAY
  PkgConfig::CLANGL
  PkgConfig::CLANGUI
  PkgConfig::CLANGUISTYLESILVER
  PkgConfig::CLANSOUND
  PkgConfig::CLANVORBIS
  OpenGL::GLU
  Threads::Threads)

install_with_wrapper(TARGETS ${PROJECT_NAME})

install(DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/data/
  DESTINATION ${CMAKE_INSTALL_DATADIR}/${PROJECT_NAME})

install(FILES
  ${PROJECT_NAME}.svg
  DESTINATION ${CMAKE_INSTALL_DATADIR}/icons/hicolor/scalable/apps)

install(FILES
  ${PROJECT_NAME}.desktop
  DESTINATION ${CMAKE_INSTALL_DATADIR}/applications)

if(BUILD_EXTRA)
  file(GLOB EXTRA_SOURCES RELATIVE ${CMAKE_CURRENT_SOURCE_DIR}
    test/*.cxx)
  foreach(SRC ${EXTRA_SOURCES})
    get_filename_component(SRC_BASENAME ${SRC} NAME_WE)
    add_executable("wst-${SRC_BASENAME}" ${SRC})
    target_link_libraries("wst-${SRC_BASENAME}"
      PkgConfig::CLANAPP
      PkgConfig::CLANDISPLAY
      PkgConfig::CLANGL
      libwindstille_collision)
    install(TARGETS "wst-${SRC_BASENAME}"
      RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR})
  endforeach()
endif()

if(BUILD_TESTS)
  # add 'make test' target, use 'make test ARGS="-V"' or 'ctest -V' for verbose
  enable_testing()
endif()

# EOF #
