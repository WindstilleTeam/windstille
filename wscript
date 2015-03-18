#! /usr/bin/env python
# -*- coding: utf-8 -*-

from glob import glob

profile_cxxflags = [ "-std=c++1y", "-O2", "-g3", "-pg" ]
profile_linkflags = [ "-pg" ]

debug_cxxflags = [ "-O0", "-g3" ]

release_cxxflags = [ "-O3", "-s" ]

developer_cxxflags = [
    # "-ansi",
    "-pedantic",
    "-Wall",
    "-Wextra",
    "-Wno-c++0x-compat",
    "-Wnon-virtual-dtor",
    "-Weffc++",
    "-Wconversion",
    "-Werror",
    "-Wshadow",
    "-Wcast-qual",
    "-Winit-self", # only works with >= -O1
    "-Wno-unused-parameter",
    # "-Winline",
    # "-Wfloat-equal",
    # "-Wunreachable-code",
]


def configure(conf):
    conf.load('g++')
    conf.load('flex bison')
    conf.load('compiler_cxx boost')

    conf.check_cxx(lib=["boost_system", "boost_filesystem"],
                   header_name="boost/filesystem.hpp",
                   uselib_store="BOOST_FILESYSTEM")
    conf.check_cxx(header_name="boost/signals2.hpp", uselib_store="BOOST_SIGNAL2")

    conf.check_cfg(atleast_pkgconfig_version='0.0.0')
    conf.check_cfg(package='sdl2', args=['--cflags', '--libs'])
    conf.check_cfg(package='SDL2_image', args=['--cflags', '--libs'])
    conf.check_cfg(package='SDL2_mixer', args=['--cflags', '--libs'])

    conf.check_cfg(package='libpng', args=['--cflags', '--libs'])
    conf.check_cxx(lib='jpeg')
    conf.check_cfg(package='gtkmm-2.4', args=['--cflags', '--libs'])
    conf.check_cfg(package='gtkglextmm-1.2', args=['--cflags', '--libs'])
    conf.check_cfg(package='freetype2', args=['--cflags', '--libs'])

    conf.check_cfg(package='openal', args=['--cflags', '--libs'])
    conf.check_cfg(package='gl', args=['--cflags', '--libs'])
    conf.check_cfg(package='glew', args=['--cflags', '--libs'])

    conf.check_cfg(package='ogg', args=['--cflags', '--libs'])
    conf.check_cfg(package='vorbis', args=['--cflags', '--libs'])
    conf.check_cfg(package='vorbisfile', args=['--cflags', '--libs'])

    conf.env.append_value("CXXFLAGS", ["-std=c++1y"])


def options(opt):
    opt.load('compiler_cxx')
    opt.load('compiler_cxx boost')


def build(bld):
    # CXXFILESUFFIX = ".cpp",
    # YACCFLAGS=['-d', '--no-lines']
    bld.program(target="miniswig",
                source=['external/miniswig/parser.yy',
                        'external/miniswig/lexer.ll',
                        'external/miniswig/create_docu.cpp',
                        'external/miniswig/xmlwriter.cpp',
                        'external/miniswig/create_wrapper.cpp',
                        'external/miniswig/main.cpp',
                        'external/miniswig/tree.cpp'],
                includes=[".", "external/miniswig/"])

    bld.stlib(target='wst_util',
              source=(glob('src/lisp/*.cpp') +
                      glob('src/util/*.cpp')),
              includes=["src/"])

    bld.stlib(target='wst_math',
              source=glob('src/math/*.cpp'),
              includes=["src/"])

    bld.stlib(target='wst_navgraph',
              source=glob('src/navigation/*.cpp'),
              includes=["src/"])

    bld.stlib(target='wst_particles',
              source=glob('src/particles/*.cpp'),
              includes=["src/"])

    bld.stlib(target='wst_sound',
              source=glob('src/sound/*.cpp'),
              includes=["src/"])

    bld.stlib(target='wst_display',
              source=(glob('src/font/*.cpp') +
                      glob('src/display/*.cpp') +
                      glob('src/scenegraph/*.cpp') +
                      glob('src/sprite2d/*.cpp') +
                      glob('src/sprite3d/*.cpp')),
              includes=["src/", '/usr/include/freetype2'],
              use=['FREETYPE2', 'SDL2', 'SDL2_IMAGE'])

    bld.stlib(target='wst_system',
              source=glob('src/system/*.cpp'),
              includes=["src/"],
              use=["SDL2"])

    bld.stlib(target='squirrel',
              source=(glob('external/SQUIRREL3/squirrel/*.cpp') +
                      glob('external/SQUIRREL3/sqstdlib/*.cpp')),
              includes=["external/SQUIRREL3/include/"],
              export_includes=["external/SQUIRREL3/include/"])

    bld.program(target='windstille',
                source=(glob('src/app/*.cpp') +
                        glob('src/armature/*.cpp') +
                        glob('src/collision/*.cpp') +
                        glob('src/engine/*.cpp') +
                        glob('src/gui/*.cpp') +
                        glob('src/hud/*.cpp') +
                        glob('src/input/*.cpp') +
                        glob('src/objects/*.cpp') +
                        glob('src/properties/*.cpp') +
                        glob('src/screen/*.cpp') +
                        glob('src/scripting/*.cpp') +
                        glob('src/tile/*.cpp')),
                includes=[".", "src/"],
                use=['wst_particles', 'wst_navgraph', 'wst_display', 'wst_util', 'wst_math', 'wst_sound',
                     'FREETYPE2',
                     'SDL2', 'SDL2_IMAGE',
                     'OPENAL', 'OGG', 'VORBIS', 'VORBISFILE',
                     'squirrel', 'LIBPNG', 'JPEG',
                     'GL', 'GLEW',
                     'BOOST_FILESYSTEM'])

    bld.program(target='windstille-editor',
                source=glob('src/editor/*.cpp'),
                includes=[".", "src/"],
                use=['wst_particles', 'wst_navgraph', 'wst_display', 'wst_util', 'wst_math', 'wst_sound',
                     'SDL2', 'SDL2_image',
                     'BOOST_FILESYSTEM',
                     'FREETYPE2',
                     'GTKGLEXTMM-1.2' , 'GTKMM-2.4', 'LIBPNG', 'GL', 'GLEW',
                     'JPEG'])


# EOF #
