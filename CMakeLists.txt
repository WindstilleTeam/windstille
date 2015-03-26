# Windstille - A Sci-Fi Action-Adventure Game
# Copyright (C) 2015 Ingo Ruhnke <grumbel@gmx.de>
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

cmake_minimum_required(VERSION 3.0)
project(galapix VERSION 0.3.0)

set(BUILD_TESTS 0 CACHE BOOL "Build tests")
set(BUILD_EXTRA 0 CACHE BOOL "Build extra apps")
set(WARNINGS 0 CACHE BOOL "Switch on extra warnings")

# TODO:
# * build data
# * build tests
# * build extra
# * Windows/Mac stuff
# * bison gets file ending wrong
# * ogg/openal/...

if(WARNINGS)
  set(WARNINGS_CXX_FLAGS
    -pedantic -Wall -Wextra -Wno-c++0x-compat -Wnon-virtual-dtor -Weffc++
    -Wconversion -Werror -Wshadow -Wcast-qual -Winit-self -Wno-unused-parameter)
  # -Winline -Wfloat-equal -Wunreachable-code
endif()

add_compile_options(-std=c++1y)

include(CheckIncludeFile)

find_package(Threads REQUIRED)
find_package(Boost COMPONENTS system filesystem signals REQUIRED)
find_package(CURL REQUIRED)
find_package(JPEG REQUIRED)
find_package(PNG REQUIRED)
find_package(Freetype REQUIRED)
find_package(BISON REQUIRED)
find_package(FLEX REQUIRED)
find_package(OpenAL REQUIRED)
find_package(OpenGL REQUIRED)
find_package(GLEW REQUIRED)

find_package(PkgConfig REQUIRED)

set(PROFILE_CXXFLAGS "-std=c++1y -O2 -g3 -pg")
set(PROFILE_LINKFLAGS "-pg")

pkg_search_module(SDL2 REQUIRED sdl2)

if(BUILD_EDITOR)
  pkg_search_module(GTKMM REQUIRED gtkmm-2.4)
  pkg_search_module(GLADEMM REQUIRED libglademm-2.4)
  pkg_search_module(GTKGLEXTMM REQUIRED gtkglextmm-1.2)

  set(WINDSTILLE_EDITOR_DEPS_INCLUDE_DIRS
    ${GTKMM_INCLUDE_DIRS}
    ${GLADEMM_INCLUDE_DIRS}
    ${GTKGLEXTMM_INCLUDE_DIRS})
  set(WINDSTILLE_EDITOR_DEPS_LIBRARIES
    ${GTKMM_LIBRARIES}
    ${GLADEMM_LIBRARIES}
    ${GTKGLEXTMM_LIBRARIES})
endif()

include_directories(.)
include_directories(src/)
include_directories(${CMAKE_CURRENT_BINARY_DIR}/src/)
include_directories(SYSTEM external/SQUIRREL3/include)
include_directories(SYSTEM external/miniswig)
# include_directories(SYSTEM external/glm-0.9.6.1/)
# include_directories(SYSTEM external/googletest/include/)

set(WINDSTILLE_DEPS_INCLUDE_DIRS
  ${SDL2_INCLUDE_DIRS}
  ${FREETYPE_INCLUDE_DIRS}
  ${OPENGL_INCLUDE_DIRS}
  ${GLEW_INCLUDE_DIRS}
  ${PNG_INCLUDE_DIRS}
  ${JPEG_INCLUDE_DIRS}
  ${Boost_INCLUDE_DIR}
  ${Boost_SIGNALS_INCLUDE_DIR}
  ${CMAKE_THREAD_LIBS_INIT})

set(WINDSTILLE_DEPS_LIBRARIES
  ${SDL2_LIBRARIES}
  ${FREETYPE_LIBRARIES}
  ${OPENGL_LIBRARIES}
  ${GLEW_LIBRARIES}
  ${PNG_LIBRARIES}
  ${JPEG_LIBRARIES}
  ${Boost_SYSTEM_LIBRARY}
  ${Boost_FILESYSTEM_LIBRARY}
  ${Boost_SIGNALS_LIBRARY}
  ${CMAKE_THREAD_LIBS_INIT})

bison_target(parser
   ${CMAKE_CURRENT_SOURCE_DIR}/external/miniswig/parser.yy
   ${CMAKE_CURRENT_BINARY_DIR}/parser.cpp
   --no-lines -d)
flex_target(MyScanner
  ${CMAKE_CURRENT_SOURCE_DIR}/external/miniswig/lexer.ll
  ${CMAKE_CURRENT_BINARY_DIR}/lexer.cpp)

file(GLOB SQUIRREL_SOURCES
  external/SQUIRREL3/squirrel/*.cpp
  external/SQUIRREL3/sqstdlib/*.cpp)
add_library(squirrel ${SQUIRREL_SOURCES})

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

# env.Depends(env.Command('src/scripting/miniswig.tmp', 'src/scripting/wrapper.interface.hpp',
#                         ["$CXX -E -Isrc/ -x c -CC $SOURCE -o $TARGET -DSCRIPTING_API"]),
#             ['src/scripting/interface.hpp',
#              'src/scripting/game_objects.hpp'])

# env.Depends(env.Command(['src/scripting/wrapper.cpp', 'src/scripting/wrapper.hpp'],
#                         'src/scripting/miniswig.tmp',
#                         ["$MINISWIG --input $SOURCE --output-cpp ${TARGETS[0]} --output-hpp ${TARGETS[1]} "+
#                          "--module windstille --select-namespace Scripting"]),
#                        miniswig_bin)

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
target_compile_options(wstlib PRIVATE ${WARNINGS_CXX_FLAGS})
target_include_directories(wstlib SYSTEM PRIVATE
  ${WINDSTILLE_DEPS_INCLUDE_DIRS})
target_link_libraries(wstlib
  ${WINDSTILLE_DEPS_LIBRARIES})

# compile galapix.sdl
if(BUILD_EDITOR)
  file(WINDSTILLE_EDITOR_SOURCES GLOB RELATIVE ${CMAKE_CURRENT_SOURCE_DIR}
    src/editor/*.cpp)

  add_executable(windstille-editor ${WINDSTILLE_EDITOR_SOURCES})
  target_compile_options(windstille-editor PRIVATE ${WARNINGS_CXX_FLAGS})
  target_include_directories(galapix.gtk SYSTEM PRIVATE
    ${WINDSTILLE_DEPS_INCLUDE_DIRS}
    ${WINDSTILLE_EDITOR_DEPS_INCLUDE_DIRS})
  target_link_libraries(windstille-editor
    wstlib
    ${WINDSTILLE_DEPS_LIBRARIES}
    ${WINDSTILLE_EDITOR_DEPS_LIBRARIES})
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
target_link_libraries(windstille
  ${WINDSTILLE_DEPS_LIBRARIES})
target_include_directories(windstille SYSTEM PRIVATE
  ${WINDSTILLE_DEPS_INCLUDE_DIRS})
target_link_libraries(windstille
  wstlib
  ${WINDSTILLE_DEPS_LIBRARIES})

# EOF #
