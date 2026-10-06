# SuperTux - miniswig build script
# Copyright (C) 2007 Timothy Goya <tuxdev103@gmail.com>
#               2020 Ingo Ruhnke <grumbel@gmail.com>
#
# This program is free software; you can redistribute it and/or modify
# it under the terms of the GNU General Public License as published by
# the Free Software Foundation; either version 2 of the License, or
# (at your option) any later version.
#
# This program is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
# GNU General Public License for more details.
#
# You should have received a copy of the GNU General Public License
# along with this program; if not, write to the Free Software
# Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA

cmake_minimum_required(VERSION 3.21)

# Source of truth: top-level VERSION file (e.g. "0.1.0-dev"), or
# -DPROJECT_VERSION_FULL=... from the packaging (Nix flake appends
# .<revCount>+g<rev>).
# A parent project's PROJECT_VERSION_FULL isn't ours when we are a subdirectory
if(NOT DEFINED PROJECT_VERSION_FULL OR NOT CMAKE_SOURCE_DIR STREQUAL CMAKE_CURRENT_SOURCE_DIR)
  file(STRINGS "${CMAKE_CURRENT_SOURCE_DIR}/VERSION" PROJECT_VERSION_FULL LIMIT_COUNT 1)
endif()

project(miniswig)

message(STATUS "miniswig version: ${PROJECT_VERSION_FULL}")

option(BUILD_TESTS "Build test cases" OFF)
option(MINISWIG_INSTALL "Generate install rules, off by default when miniswig is a subdirectory" ${PROJECT_IS_TOP_LEVEL})

if(BUILD_TESTS)
  # add 'make test' target, use 'make test ARGS="-V"' or 'ctest -V' for verbose
  enable_testing()
endif(BUILD_TESTS)

find_package(BISON REQUIRED)
find_package(FLEX REQUIRED)

bison_target(MyParser src/parser.yy
  ${CMAKE_CURRENT_BINARY_DIR}/parser.cpp
  )

flex_target(MyScanner src/lexer.ll
  ${CMAKE_CURRENT_BINARY_DIR}/lexer.cpp
  )

add_flex_bison_dependency(MyScanner MyParser)

add_library(miniswig_generated STATIC
  ${BISON_MyParser_OUTPUTS}
  ${FLEX_MyScanner_OUTPUTS}
  )
target_include_directories(miniswig_generated PUBLIC src/)
set_target_properties(miniswig_generated PROPERTIES CXX_CLANG_TIDY "")

# Old versions of flex will generate forbidden 'register' keyword, disable this warning
# https://github.com/westes/flex/commit/8091dc907663673d9e0295c57fa446bdd38e9fab
set_target_properties(miniswig_generated PROPERTIES COMPILE_FLAGS "-Wno-register")

add_executable(miniswig
  src/create_docu.cpp
  src/create_wrapper.cpp
  src/main.cpp
  src/tree.cpp
  src/xmlwriter.cpp
  )
target_link_libraries(miniswig miniswig_generated)
target_include_directories(miniswig PUBLIC src/)
target_compile_definitions(miniswig PRIVATE MINISWIG_VERSION="${PROJECT_VERSION_FULL}")

if(MINISWIG_INSTALL)
  install(TARGETS miniswig)
endif()

if(BUILD_TESTS)
  find_package(squirrel 3.2 REQUIRED)

  file(MAKE_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}/tests)

  # run the C++ preprocessor
  add_custom_command(VERBATIM
    OUTPUT ${CMAKE_CURRENT_BINARY_DIR}/tests/example.ii
    DEPENDS ${CMAKE_CURRENT_SOURCE_DIR}/tests/example.hpp

    COMMAND ${CMAKE_CXX_COMPILER}
    ARGS
    -x "c++" -E -CC
    ${CMAKE_CURRENT_SOURCE_DIR}/tests/example.hpp
    -o ${CMAKE_CURRENT_BINARY_DIR}/tests/example.ii
    -I${CMAKE_CURRENT_SOURCE_DIR}/tests
    -DSCRIPTING_API
    )

  # run miniswig to generate the wrapper code
  add_custom_command(VERBATIM
    OUTPUT
    ${CMAKE_CURRENT_BINARY_DIR}/tests/example_wrap.cpp
    ${CMAKE_CURRENT_BINARY_DIR}/tests/example_wrap.hpp

    DEPENDS ${CMAKE_CURRENT_BINARY_DIR}/tests/example.ii

    COMMAND $<TARGET_FILE:miniswig>
    ARGS
    --input ${CMAKE_CURRENT_BINARY_DIR}/tests/example.ii
    --output-cpp ${CMAKE_CURRENT_BINARY_DIR}/tests/example_wrap.cpp
    --output-hpp ${CMAKE_CURRENT_BINARY_DIR}/tests/example_wrap.hpp
    --module example
    --select-namespace example
    )

  # build the test executable
  add_executable(script_test
    tests/script_test.cpp
    ${CMAKE_CURRENT_BINARY_DIR}/tests/example_wrap.cpp
    tests/example.cpp
    )
  target_include_directories(script_test PUBLIC
    include/
    tests/
    ${CMAKE_CURRENT_BINARY_DIR}/tests/
    )
  target_link_libraries(script_test PRIVATE
    squirrel::squirrel
    squirrel::sqstdlib
    )

  # Scripts that exercise pure Squirrel language features / the harness
  set(SCRIPT_TESTS
    helloworld
    closure
    environment
    exception
    test
    example
  )

  foreach(script ${SCRIPT_TESTS})
    add_test(NAME ${script}_test
      COMMAND $<TARGET_FILE:script_test> ${CMAKE_CURRENT_SOURCE_DIR}/tests/${script}.nut
    )
  endforeach()
endif()

# EOF #
