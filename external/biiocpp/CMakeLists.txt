# biio - Binary file I/O
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

cmake_minimum_required(VERSION 4.0)
project(biio VERSION 0.1.0)

set(TINYCMMC_MODULE_PATH "${CMAKE_CURRENT_LIST_DIR}/external/cmake-modules/modules/")
find_package(tinycmmc CONFIG)
message(STATUS "tinycmmc module path: ${TINYCMMC_MODULE_PATH}")
list(APPEND CMAKE_MODULE_PATH ${TINYCMMC_MODULE_PATH})

option(BUILD_TESTS "Build test cases" OFF)

include(GNUInstallDirs)
include(MaximumWarnings)
include(ClangTidy)

file(GLOB BIIO_HEADER_SOURCES RELATIVE ${CMAKE_CURRENT_SOURCE_DIR}
  include/biio/*.hpp)
file(GLOB BIIO_SOURCES RELATIVE ${CMAKE_CURRENT_SOURCE_DIR}
  src/*.cpp)

add_library(biio STATIC ${BIIO_SOURCES})
set_target_properties(biio PROPERTIES PUBLIC_HEADER "${BIIO_HEADER_SOURCES}")
set_target_properties(biio PROPERTIES
  CXX_STANDARD 20
  CXX_STANDARD_REQUIRED ON
  CXX_EXTENSIONS OFF)
target_compile_options(biio PRIVATE ${WARNINGS_CXX_FLAGS})
target_include_directories(biio INTERFACE
  $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
  $<INSTALL_INTERFACE:include>)
target_include_directories(biio PRIVATE src/ include/)

if(BUILD_TESTS)
  if(NOT TARGET GTest)
    find_package(GTest REQUIRED)
  endif()

  file(GLOB TEST_BIIO_SOURCES test/*_test.cpp)
  add_executable(test_biio ${TEST_BIIO_SOURCES})
  set_target_properties(test_biio PROPERTIES
    CXX_STANDARD 20
    CXX_STANDARD_REQUIRED ON
    CXX_EXTENSIONS OFF)
  target_compile_options(test_biio PRIVATE ${WARNINGS_CXX_FLAGS})
  target_include_directories(test_biio PRIVATE src/)
  target_link_libraries(test_biio
    biio
    GTest::GTest
    GTest::Main)

  enable_testing()
  add_test(NAME test_biio
    WORKING_DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}"
    COMMAND test_biio)
endif()

include(ExportAndInstallLibrary)

# EOF #
