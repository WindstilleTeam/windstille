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

  # Resolve dependencies from the top-level external/ tree (or a nested
  # copy).  Package names and directory names are not always identical.
  macro(_wstgui_add_dep _pkg _dir)
    find_package(${_pkg} QUIET)
    if(NOT TARGET ${_pkg}::${_pkg})
      set(_cands
        "${CMAKE_SOURCE_DIR}/external/${_dir}"
        "${CMAKE_CURRENT_SOURCE_DIR}/external/${_dir}")
      set(_found FALSE)
      foreach(_c IN LISTS _cands)
        if(EXISTS "${_c}/CMakeLists.txt")
          get_property(_already GLOBAL PROPERTY "tinycmmc_added_${_dir}" SET)
          if(_already)
            message(STATUS "${_pkg} already added from external/${_dir}")
          else()
            message(STATUS "${_pkg} not found via find_package, using ${_c}")
            add_subdirectory("${_c}" "${CMAKE_BINARY_DIR}/external/${_dir}" EXCLUDE_FROM_ALL)
            set_property(GLOBAL PROPERTY "tinycmmc_added_${_dir}" TRUE)
          endif()
          set(_found TRUE)
          break()
        endif()
      endforeach()
      if(NOT _found)
        message(FATAL_ERROR
          "${_pkg} not found and external/${_dir} is missing under both "
          "the top-level source tree and the current project.")
      endif()
    endif()
  endmacro()

  _wstgui_add_dep(sexp sexpcpp)
  _wstgui_add_dep(babyxml babyxml)
  _wstgui_add_dep(logmich logmich)
  _wstgui_add_dep(geom geomcpp)
  _wstgui_add_dep(prio prio)
  _wstgui_add_dep(surf surfcpp)
  _wstgui_add_dep(wstdisplay wstdisplay)
  _wstgui_add_dep(wstinput wstinput)
  _wstgui_add_dep(wstsound wstsound)
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
