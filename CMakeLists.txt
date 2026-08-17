# babyxml - tiny parser for XML-like dialect
# Copyright (C) 2009-2020 Ingo Ruhnke <grumbel@gmail.com>
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

cmake_minimum_required(VERSION 3.6)
project(babyxml VERSION 0.0.0)

include(mk/cmake/TinyCMMC.cmake)

add_library(babyxml STATIC
  src/babyxml.cpp
  src/reader.cpp)
target_include_directories(babyxml PUBLIC
  $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
  $<INSTALL_INTERFACE:include>)
target_compile_options(babyxml PRIVATE ${TINYCMMC_WARNINGS_CXX_FLAGS})
set_target_properties(babyxml PROPERTIES
  PUBLIC_HEADER "include/babyxml.hpp")

if(BUILD_EXTRA)
  add_executable(babyxml_exe extra/babyxml.cpp)
  set_target_properties(babyxml_exe PROPERTIES OUTPUT_NAME "babyxml")
  target_link_libraries(babyxml_exe PRIVATE babyxml)
  target_compile_options(babyxml_exe PRIVATE ${TINYCMMC_WARNINGS_CXX_FLAGS})
  install(TARGETS babyxml_exe
    RUNTIME DESTINATION ${CMAKE_INSTALL_FULL_BINDIR})
endif()

tinycmmc_export_and_install_library(babyxml)

# EOF #
