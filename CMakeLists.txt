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

cmake_policy(SET CMP0078 OLD) # Swig
cmake_policy(SET CMP0086 OLD) # Swig

option(WARNINGS "Switch on extra warnings" OFF)
option(WERROR "Treat all warnings as errors" OFF)
option(BUILD_TESTS "Build test cases" OFF)
option(BUILD_EXTRA "Build extras" OFF)

include(GNUInstallDirs)

if(WARNINGS)
endif()

if(WERROR)
endif()

if (BUILD_TESTS)
  # add 'make test' target, use 'make test ARGS="-V"' or 'ctest -V' for verbose
  enable_testing()
endif(BUILD_TESTS)

find_package(Git)
if(GIT_FOUND AND EXISTS "${CMAKE_SOURCE_DIR}/.git")
  execute_process(COMMAND ${GIT_EXECUTABLE} describe --tags --always HEAD
    OUTPUT_VARIABLE GIT_REPO_VERSION
    OUTPUT_STRIP_TRAILING_WHITESPACE)

  execute_process(COMMAND ${GIT_EXECUTABLE} log -n 1 --pretty=%cd --date=format:%Y-%m-%d ${GIT_REPO_VERSION}
    OUTPUT_VARIABLE GIT_REPO_DATE
    OUTPUT_STRIP_TRAILING_WHITESPACE)
else()
  set(GIT_REPO_VERSION "${PROJECT_VERSION}")
  string(TIMESTAMP GIT_REPO_DATE "%Y-%m-%d")
endif()

include_directories(src/)
include_directories(src/scripting/)

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

include_directories(${GUILE_INCLUDE_DIRS})
set_source_files_properties(src/scripting/windstille.i PROPERTIES CPLUSPLUS ON)
swig_add_library(libwindstille_scripting TYPE STATIC LANGUAGE guile SOURCES src/scripting/windstille.i)

file(GLOB LIBWINDSTILLE_COLLISION_SOURCES
  src/collision/*.cxx)

add_library(libwindstille_collision STATIC ${LIBWINDSTILLE_COLLISION_SOURCES})
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

install(TARGETS ${PROJECT_NAME} RUNTIME
  DESTINATION ${CMAKE_INSTALL_LIBEXECDIR})

configure_file(
  ${CMAKE_CURRENT_SOURCE_DIR}/${PROJECT_NAME}.sh.in
  ${CMAKE_BINARY_DIR}/${PROJECT_NAME}.sh)

install(FILES
  ${CMAKE_BINARY_DIR}/${PROJECT_NAME}.sh
  RENAME ${PROJECT_NAME}
  PERMISSIONS OWNER_EXECUTE OWNER_READ OWNER_WRITE GROUP_READ GROUP_EXECUTE WORLD_READ WORLD_EXECUTE
  DESTINATION ${CMAKE_INSTALL_BINDIR})

install(DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/data/
  DESTINATION ${CMAKE_INSTALL_DATADIR}/${PROJECT_NAME})

install(FILES
  ${PROJECT_NAME}.svg
  DESTINATION ${CMAKE_INSTALL_DATADIR}/icons/hicolor/scalable/apps)

install(FILES
  ${PROJECT_NAME}.desktop
  DESTINATION ${CMAKE_INSTALL_DATADIR}/applications)

configure_file(${PROJECT_NAME}.appdata.xml.in ${CMAKE_CURRENT_BINARY_DIR}/${PROJECT_NAME}.appdata.xml)
install(FILES ${CMAKE_CURRENT_BINARY_DIR}/${PROJECT_NAME}.appdata.xml
  DESTINATION ${CMAKE_INSTALL_DATADIR}/metainfo)

if(BUILD_EXTRA)
  file(GLOB EXTRA_SOURCES RELATIVE ${CMAKE_CURRENT_SOURCE_DIR}
    test/*.cxx)
  foreach(SRC ${EXTRA_SOURCES})
    get_filename_component(SRC_BASENAME ${SRC} NAME_WE)
    add_executable(${SRC_BASENAME} ${SRC})
    target_link_libraries(${SRC_BASENAME}
      PkgConfig::CLANAPP
      PkgConfig::CLANDISPLAY
      PkgConfig::CLANGL
      libwindstille_collision)
  endforeach(SRC)
endif()

if(BUILD_TESTS)
  add_test(NAME ${PROJECT_NAME}.appdata.xml
    WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}
    COMMAND appstream-util validate-relax ${CMAKE_CURRENT_BINARY_DIR}/${PROJECT_NAME}.appdata.xml)
endif(BUILD_TESTS)

# EOF #
