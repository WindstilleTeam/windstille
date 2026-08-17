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
    add_subdirectory("${CMAKE_CURRENT_SOURCE_DIR}/external/tinycmmc"
                     "${CMAKE_CURRENT_BINARY_DIR}/external/tinycmmc"
                     EXCLUDE_FROM_ALL)
    set(tinycmmc_DIR "${CMAKE_CURRENT_BINARY_DIR}/external/tinycmmc")
    # The generated Config.cmake points TINYCMMC_MODULE_PATH at the
    # *install* location; override it with the in-tree modules path
    # that is actually usable during the build.
    set(TINYCMMC_MODULE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/external/tinycmmc/modules/")
    find_package(tinycmmc CONFIG REQUIRED)
    # Re-assert the in-tree path (find_package may have overwritten it)
    set(TINYCMMC_MODULE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/external/tinycmmc/modules/")
    message(STATUS "tinycmmc module path (in-tree): ${TINYCMMC_MODULE_PATH}")
    list(APPEND CMAKE_MODULE_PATH ${TINYCMMC_MODULE_PATH})
  endif()
endif()

include(TinyCMMC)

# EOF #
