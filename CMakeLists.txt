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
project(wstgui VERSION 0.3.0)

set(TINYCMMC_MODULE_PATH "${CMAKE_CURRENT_LIST_DIR}/external/tinycmmc/modules/")
find_package(tinycmmc CONFIG)
message(STATUS "tinycmmc module path: ${TINYCMMC_MODULE_PATH}")
list(APPEND CMAKE_MODULE_PATH ${TINYCMMC_MODULE_PATH})

include(GetProjectVersion)
include(GNUInstallDirs)
include(ClangTidy)
include(MaximumWarnings)

find_package(PkgConfig REQUIRED)

pkg_search_module(SDL2 REQUIRED sdl2 IMPORTED_TARGET)
pkg_search_module(SIGCXX REQUIRED sigc++-2.0 IMPORTED_TARGET)

# Build dependencies
function(build_dependencies)
  set(BUILD_TESTS OFF)
  set(BUILD_EXTRA OFF)

  find_package(sexp QUIET)
  if(NOT TARGET sexp::sexp)
    add_subdirectory(external/sexpcpp/ EXCLUDE_FROM_ALL)
  endif()

  find_package(babyxml QUIET)
  if(NOT TARGET babyxml::babyxml)
    add_subdirectory(external/babyxml/ EXCLUDE_FROM_ALL)
  endif()

  find_package(logmich QUIET)
  if(NOT TARGET logmich::logmich)
    add_subdirectory(external/logmich/ EXCLUDE_FROM_ALL)
  endif()

  find_package(geom QUIET)
  if(NOT TARGET geom::geom)
    add_subdirectory(external/geomcpp/ EXCLUDE_FROM_ALL)
  endif()

  find_package(prio QUIET)
  if(NOT TARGET prio::prio)
    add_subdirectory(external/priocpp/ EXCLUDE_FROM_ALL)
  endif()

  find_package(surf QUIET)
  if(NOT TARGET surf::surf)
    add_subdirectory(external/surfcpp/ EXCLUDE_FROM_ALL)
  endif()

  find_package(wstdisplay QUIET)
  if(NOT TARGET wstdisplay::wstdisplay)
    add_subdirectory(external/wstdisplay/ EXCLUDE_FROM_ALL)
  endif()

  find_package(wstinput QUIET)
  if(NOT TARGET wstinput::wstinput)
    add_subdirectory(external/wstinput/ EXCLUDE_FROM_ALL)
  endif()

  find_package(wstsound QUIET)
  if(NOT TARGET wstsound::wstsound)
    add_subdirectory(external/wstsound/ EXCLUDE_FROM_ALL)
  endif()
endfunction()
build_dependencies()

file(GLOB WSTGUI_HEADERS RELATIVE ${CMAKE_CURRENT_SOURCE_DIR}
  include/wstgui/*.hpp)
file(GLOB WSTGUI_SOURCES RELATIVE ${CMAKE_CURRENT_SOURCE_DIR}
  src/*.cpp)

add_library(wstgui STATIC ${WSTGUI_SOURCES})
set_target_properties(wstgui PROPERTIES PUBLIC_HEADER "${WSTGUI_HEADERS}")
target_compile_options(wstgui PUBLIC ${WARNINGS_CXX_FLAGS})
set_target_properties(wstgui PROPERTIES
  CXX_STANDARD 20
  CXX_STANDARD_REQUIRED ON
  CXX_EXTENSIONS OFF)
target_include_directories(wstgui SYSTEM PUBLIC ${OPENAL_INCLUDE_DIR})
target_include_directories(wstgui PUBLIC
  $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
  $<INSTALL_INTERFACE:include>
  )
target_include_directories(wstgui PRIVATE
  src/
  include/wstgui/
  )
target_link_libraries(wstgui PUBLIC
  wstinput::wstinput
  wstsound::wstsound
  geom::geom
  prio::prio
  PkgConfig::SIGCXX
  PkgConfig::SDL2
  wstdisplay::wstdisplay
  )

if(CWIID_LIBRARY)
  target_compile_options(wstgui PUBLIC -DHAVE_CWIID)
  target_link_libraries(wstgui ${CWIID_LIBRARY})
endif()

install(TARGETS wstgui
  EXPORT wstgui-targets
  LIBRARY DESTINATION "${CMAKE_INSTALL_LIBDIR}"
  PUBLIC_HEADER DESTINATION "${CMAKE_INSTALL_INCLUDEDIR}/${PROJECT_NAME}")

if(BUILD_EXTRA)
  file(GLOB EXTRA_SOURCES RELATIVE ${CMAKE_CURRENT_SOURCE_DIR}
    extra/*.cpp)
  foreach(SRC ${EXTRA_SOURCES})
    get_filename_component(SRC_BASENAME ${SRC} NAME_WE)
    add_executable("${SRC_BASENAME}_exe" ${SRC})
    set_target_properties("${SRC_BASENAME}_exe" PROPERTIES
      OUTPUT_NAME "${SRC_BASENAME}"
      CXX_STANDARD 20
      CXX_STANDARD_REQUIRED ON
      CXX_EXTENSIONS OFF)
    target_link_libraries("${SRC_BASENAME}_exe" wstgui)
    install(TARGETS "${SRC_BASENAME}_exe"
      RUNTIME DESTINATION "${CMAKE_INSTALL_BINDIR}")
  endforeach(SRC)
endif()

include(ExportAndInstallLibrary)

# EOF #
