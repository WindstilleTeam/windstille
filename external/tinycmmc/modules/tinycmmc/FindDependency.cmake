# Copyright (C) 2022 Ingo Ruhnke <grumbel@gmail.com>
#
# This software is provided 'as-is', without any express or implied
# warranty.  In no event will the authors be held liable for any damages
# arising from the use of this software.
#
# Permission is granted to anyone to use this software for any purpose,
# including commercial applications, and to alter it and redistribute it
# freely, subject to the following restrictions:
#
# 1. The origin of this software must not be misrepresented; you must not
#    claim that you wrote the original software. If you use this software
#    in a product, an acknowledgment in the product documentation would be
#    appreciated but is not required.
# 2. Altered source versions must be plainly marked as such, and must not be
#    misrepresented as being the original software.
# 3. This notice may not be removed or altered from any source distribution.

# Look for a dependency either as an already-found CMake package or as
# a directory under external/.  Directory names are not always identical
# to the package/module name (e.g. "strut" vs "strutcpp", "geom" vs
# "geomcpp"), so several candidates are tried.  Search is performed both
# relative to the calling project and relative to the top-level source
# tree so that a single copy of each dependency under the root external/
# is sufficient.
#
# Each source directory is added at most once (tracked via a global
# property) so that multiple callers requesting the same dependency do
# not collide on the binary directory.

macro(tinycmmc_find_dependency _NAME)
  find_package(${_NAME} QUIET)
  if(${${_NAME}_FOUND})
    message(STATUS "Found ${_NAME}: ${${_NAME}_DIR}")
  else()
    set(_tinycmmc_dep_found FALSE)
    set(_tinycmmc_dep_candidates "${_NAME}" "${_NAME}cpp")

    foreach(_cand IN LISTS _tinycmmc_dep_candidates)
      foreach(_root IN ITEMS "${CMAKE_SOURCE_DIR}" "${CMAKE_CURRENT_SOURCE_DIR}")
        set(_tinycmmc_dep_path "${_root}/external/${_cand}")
        if(NOT EXISTS "${_tinycmmc_dep_path}/CMakeLists.txt")
          continue()
        endif()

        # Already added by a previous call?
        get_property(_tinycmmc_added GLOBAL PROPERTY "tinycmmc_added_${_cand}" SET)
        if(_tinycmmc_added)
          message(STATUS "Package ${_NAME} already added from external/${_cand}")
          set(_tinycmmc_dep_found TRUE)
          break()
        endif()

        message(STATUS "Package ${_NAME} not found, using ${_tinycmmc_dep_path}")
        set(BUILD_TESTS OFF)
        add_subdirectory("${_tinycmmc_dep_path}"
                         "${CMAKE_BINARY_DIR}/external/${_cand}"
                         EXCLUDE_FROM_ALL)
        set_property(GLOBAL PROPERTY "tinycmmc_added_${_cand}" TRUE)
        set(_tinycmmc_dep_found TRUE)
        break()
      endforeach()
      if(_tinycmmc_dep_found)
        break()
      endif()
    endforeach()

    if(NOT _tinycmmc_dep_found)
      message(FATAL_ERROR
        "The dependency \"${_NAME}\" could not be found (tried external/"
        "${_NAME} and external/${_NAME}cpp under both the top-level source "
        "tree and the current project).  Add it as a subtree under "
        "external/ or install a CMake package that provides it.\n")
    endif()

    unset(_tinycmmc_dep_found)
    unset(_tinycmmc_dep_candidates)
    unset(_tinycmmc_dep_path)
    unset(_tinycmmc_added)
  endif()
endmacro()

# EOF #
