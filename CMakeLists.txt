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

cmake_minimum_required(VERSION 3.21)

# Source of truth: top-level VERSION (e.g. "0.1.0-dev"), or
# -DPROJECT_VERSION_FULL=... from packaging (Nix may append .<revCount>+g<rev>).
# A parent project's PROJECT_VERSION_FULL isn't ours when we are a subdirectory
if(NOT DEFINED PROJECT_VERSION_FULL OR NOT CMAKE_SOURCE_DIR STREQUAL CMAKE_CURRENT_SOURCE_DIR)
  file(STRINGS "${CMAKE_CURRENT_SOURCE_DIR}/VERSION" PROJECT_VERSION_FULL LIMIT_COUNT 1)
endif()

# project() needs a numeric version; strip -dev / +git / similar suffixes.
string(REGEX MATCH "^[0-9]+(\\.[0-9]+)*" _project_version_cmake "${PROJECT_VERSION_FULL}")

project(biio VERSION ${_project_version_cmake} LANGUAGES CXX)

include(GNUInstallDirs)
include(CMakePackageConfigHelpers)

option(BUILD_TESTS "Build test cases" OFF)
option(BIIO_INSTALL "Generate install and export rules, off by default when biio is a subdirectory" ${PROJECT_IS_TOP_LEVEL})
option(WARNINGS "Enable extra compiler warnings" OFF)
option(WERROR "Treat warnings as errors" OFF)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

set(WARNINGS_CXX_FLAGS "")
if(WERROR)
  list(APPEND WARNINGS_CXX_FLAGS -Werror)
endif()
if(WARNINGS)
  list(APPEND WARNINGS_CXX_FLAGS -Wall -Wextra -pedantic)
endif()

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
target_include_directories(biio PUBLIC
  $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
  $<INSTALL_INTERFACE:include>)
target_include_directories(biio PRIVATE src/)
add_library(biio::biio ALIAS biio)

if(BUILD_TESTS)
  find_package(GTest REQUIRED)

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

if(BIIO_INSTALL)
  # Export and install library (inlined from former tinycmmc)
  install(TARGETS biio
    EXPORT biio
    ARCHIVE DESTINATION ${CMAKE_INSTALL_LIBDIR}
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
    PUBLIC_HEADER DESTINATION "${CMAKE_INSTALL_INCLUDEDIR}/biio")

  configure_package_config_file(
    "${CMAKE_CURRENT_SOURCE_DIR}/biio-config.cmake.in"
    "${CMAKE_CURRENT_BINARY_DIR}/biio-config.cmake"
    INSTALL_DESTINATION "${CMAKE_INSTALL_LIBDIR}/cmake/biio")
  write_basic_package_version_file(
    "${CMAKE_CURRENT_BINARY_DIR}/biio-config-version.cmake"
    VERSION "${PROJECT_VERSION}"
    COMPATIBILITY SameMinorVersion)
  export(EXPORT biio
    NAMESPACE biio::
    FILE "${CMAKE_CURRENT_BINARY_DIR}/biio-targets.cmake")
  install(EXPORT biio
    FILE biio-targets.cmake
    NAMESPACE biio::
    DESTINATION "${CMAKE_INSTALL_LIBDIR}/cmake/biio")
  install(FILES
    "${CMAKE_CURRENT_BINARY_DIR}/biio-config-version.cmake"
    "${CMAKE_CURRENT_BINARY_DIR}/biio-config.cmake"
    DESTINATION "${CMAKE_INSTALL_LIBDIR}/cmake/biio")
endif()

# EOF #
