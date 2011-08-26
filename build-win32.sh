#!/bin/bash

scons \
  CXX='i586-mingw32msvc-c++',
  CPPPATH='/home/ingo/projects/mingw32/run/include'
  LIBPATH='/home/ingo/projects/mingw32/run/lib'
  PROGSUFFIX='.exe'

# EOF #
