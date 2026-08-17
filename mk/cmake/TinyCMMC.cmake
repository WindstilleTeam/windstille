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

# Bootstrap file that is looking for where tinycmmc is installed

find_package(tinycmmc CONFIG QUIET)
if(tinycmmc_FOUND)
  message(STATUS "tinycmmc module path: ${TINYCMMC_MODULE_PATH}")
  list(APPEND CMAKE_MODULE_PATH ${TINYCMMC_MODULE_PATH})
else()
  if(NOT EXISTS "${CMAKE_CURRENT_SOURCE_DIR}/external/tinycmmc/CMakeLists.txt")
    message(FATAL_ERROR
      "The git submodule \"external/tinycmmc\" could not be found. "
      "To retrieve it, run:\n"
      "    git submodule update --init --recursive\n")
  else()
    # Expose the local subtree as a findable CMake package so that
    # sub-projects (argpp, babyxml, geomcpp, ...) can call
    # find_package(tinycmmc CONFIG) successfully without requiring a
    # nested external/tinycmmc of their own.
    set(_tinycmmc_src "${CMAKE_CURRENT_SOURCE_DIR}/external/tinycmmc")
    set(_tinycmmc_bin "${CMAKE_CURRENT_BINARY_DIR}/external/tinycmmc")
    add_subdirectory("${_tinycmmc_src}" "${_tinycmmc_bin}" EXCLUDE_FROM_ALL)

    # The generated Config.cmake records the *install* module path
    # (under CMAKE_INSTALL_PREFIX).  Rewrite it so that consumers that
    # call find_package(tinycmmc) during this configure get the in-tree
    # modules directory instead.
    set(_tinycmmc_modules "${_tinycmmc_src}/modules")
    file(WRITE "${_tinycmmc_bin}/tinycmmcConfig.cmake"
"# Generated for in-tree use by Windstille
set(TINYCMMC_MODULE_PATH \"${_tinycmmc_modules}\")
set(tinycmmc_FOUND TRUE)
")
    # Minimal version file so find_package(... REQUIRED) is happy
    file(WRITE "${_tinycmmc_bin}/tinycmmcConfigVersion.cmake"
"set(PACKAGE_VERSION \"0.1.0\")
set(PACKAGE_VERSION_EXACT TRUE)
set(PACKAGE_VERSION_COMPATIBLE TRUE)
set(PACKAGE_VERSION_UNSUITABLE FALSE)
")

    set(tinycmmc_DIR "${_tinycmmc_bin}")
    set(TINYCMMC_MODULE_PATH "${_tinycmmc_modules}")
    find_package(tinycmmc CONFIG REQUIRED)
    # Defend against anything that may have overwritten the path
    set(TINYCMMC_MODULE_PATH "${_tinycmmc_modules}" CACHE PATH "tinycmmc CMake modules" FORCE)
    list(APPEND CMAKE_MODULE_PATH "${TINYCMMC_MODULE_PATH}")
    message(STATUS "tinycmmc module path (in-tree): ${TINYCMMC_MODULE_PATH}")

    unset(_tinycmmc_src)
    unset(_tinycmmc_bin)
    unset(_tinycmmc_modules)
  endif()
endif()

include(TinyCMMC)

# EOF #
