# argpp
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

cmake_minimum_required(VERSION 3.10)
project(argpp VERSION 2.0.0)

include(mk/cmake/TinyCMMC.cmake)

function(build_dependencies)
  find_package(fmt REQUIRED)
  tinycmmc_find_dependency(strut)
endfunction()
build_dependencies()

file(GLOB ARGPP_SOURCES src/*.cpp)
file(GLOB ARGPP_HEADERS include/argpp/*.hpp)
add_library(argpp STATIC
  ${ARGPP_SOURCES})
set_target_properties(argpp PROPERTIES
  CXX_STANDARD 20
  CXX_STANDARD_REQUIRED ON
  CXX_EXTENSIONS OFF)
# target_include_directories(argpp PRIVATE include/argpp/ src/)
target_include_directories(argpp PUBLIC
  $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/src>
  $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
  $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include/argpp>
  $<INSTALL_INTERFACE:include>)
target_compile_options(argpp PRIVATE ${WARNINGS_CXX_FLAGS})
target_link_libraries(argpp PUBLIC fmt::fmt strut)
set_target_properties(argpp PROPERTIES PUBLIC_HEADER
  "${ARGPP_HEADERS}")

if(BUILD_TESTS)
  enable_testing()

  file(GLOB EXAMPLE_ARGPP_SOURCES test/*_example[0-9].cpp)
  foreach(SRC ${EXAMPLE_ARGPP_SOURCES})
    get_filename_component(SRC_BASENAME ${SRC} NAME_WE)
    add_executable(${SRC_BASENAME} ${SRC})
    set_target_properties(${SRC_BASENAME} PROPERTIES
      CXX_STANDARD 20
      CXX_STANDARD_REQUIRED ON
      CXX_EXTENSIONS OFF)
    target_link_libraries(${SRC_BASENAME} argpp)
    target_compile_options(${SRC_BASENAME} PRIVATE ${WARNINGS_CXX_FLAGS})

    add_test(NAME ${SRC_BASENAME}
      WORKING_DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}"
      COMMAND "${SRC_BASENAME}")
  endforeach()

  find_package(GTest REQUIRED)

  file(GLOB TEST_ARGPP_SOURCES test/*_test.cpp)
  add_executable(test_argpp ${TEST_ARGPP_SOURCES})
  set_target_properties(test_argpp PROPERTIES
    CXX_STANDARD 20
    CXX_STANDARD_REQUIRED ON
    CXX_EXTENSIONS OFF)
  target_compile_options(test_argpp PRIVATE ${WARNINGS_CXX_FLAGS})
  target_link_libraries(test_argpp
    argpp
    GTest::GTest
    GTest::Main)

    add_test(NAME test_argpp
      WORKING_DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}"
      COMMAND test_argpp)
endif()

tinycmmc_export_and_install_library(argpp)

# EOF #
