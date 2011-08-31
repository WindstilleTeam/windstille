##  -*- python -*-
##  Windstille - A Sci-Fi Action-Adventure Game
##  Copyright (C) 2000,2005,2011 Ingo Ruhnke <grumbel@gmx.de>
##
##  This program is free software: you can redistribute it and/or modify
##  it under the terms of the GNU General Public License as published by
##  the Free Software Foundation, either version 3 of the License, or
##  (at your option) any later version.
##  
##  This program is distributed in the hope that it will be useful,
##  but WITHOUT ANY WARRANTY; without even the implied warranty of
##  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
##  GNU General Public License for more details.
##  
##  You should have received a copy of the GNU General Public License
##  along with this program.  If not, see <http://www.gnu.org/licenses/>.

import time
import os
import sys
import scons_tests
from scons_enhancements import *
from pprint import pprint

start_time = time.time()

CacheDir('cache')

class Project:
    def __init__(self):
        self.features = {
            'platform' : sys.platform,
            '64bit' : False,
            'cwiid' : False
            }

        self.env = Environment(ENV=os.environ)

    def configure(self):
        # FIXME: None of the options are used, as only self.features
        # makes it across function calls

        # FIXME: Giving multiple CCFLAGS doesn't work since they have to be
        # broken down to a list
        opts = Variables(['custom.py'], ARGUMENTS)
        opts.Add('CPPPATH', 'Additional preprocessor paths')
        opts.Add('CPPFLAGS', 'Additional preprocessor flags')
        opts.Add('CPPDEFINES', 'defined constants')
        opts.Add('LIBPATH', 'Additional library paths')
        opts.Add('LIBS', 'Additional libraries')
        opts.Add('CCFLAGS', 'C Compiler flags')
        opts.Add('CXXFLAGS', 'C++ Compiler flags')
        opts.Add('LINKFLAGS', 'Linker Compiler flags')
        opts.Add('CC', 'C Compiler')
        opts.Add('CXX', 'C++ Compiler')
        opts.Add('BUILD', 'Build type: release, profile, debug, development')
        opts.Update(self.env)
        # opts.Save('options.cache', self.env)
        Help(opts.GenerateHelpText(self.env))

        conf = Configure(self.env, custom_tests = { 'Check32bit' : scons_tests.Check32bit,
                                                    'CheckYacc'  : scons_tests.CheckYacc,
                                                    'CheckLex'   : scons_tests.CheckLex})

        if self.env['CXX']:
            print "Using C++ compiler...", self.env['CXX']
        else:
            print "Error: C++ compiler missing"
            Exit(1)

        self.cfg = Configuration() # ("i586-mingw32msvc")
        Configuration.cfg = self.cfg
        cfg = self.cfg

        # PATH="/home/ingo/projects/mingw32/run/bin:$PATH"
        # PKG_CONFIG_LIBDIR="/home/ingo/projects/mingw32/run/lib/pkgconfig:/home/ingo/projects/mingw32/run/opt/gtk/lib/pkgconfig"

        # preset compiler flags
        cfg.Package('release', CCFLAGS = [ "-O3", "-s" ])
        cfg.Package('profile', CCFLAGS = [ "-O2", "-g3", "-pg" ], LINKFLAGS = [ "-pg" ] )
        cfg.Package('debug',   CCFLAGS = [ "-O0", "-g3" ] )
        cfg.Package('development',
                    CXX=[ "g++-svn" ],
                    CCFLAGS = [ "-O2", "-g3",
                                "-ansi",
                                "-pedantic",
                                "-Wall",
                                "-Wextra",
                                "-Wnon-virtual-dtor",
                                "-Weffc++",
                                "-Wconversion",
                                "-Wold-style-cast",
                                "-Werror",
                                "-Wshadow",
                                "-Wcast-qual",
                                "-Winit-self", # only works with >= -O1
                                "-Wno-unused-parameter",
                                # "-Winline",
                                # "-Wfloat-equal",
                                # "-Wunreachable-code",
                                ] )

        if 'BUILD' in self.env:
            print "Build Type: %s" % self.env['BUILD']
            cfg['default'] = cfg[self.env['BUILD']]
        else:
            print "Build Type: release"
            cfg['default'] = cfg['release']

        if self.features['platform'] == 'win32':
            cfg.Package('default',
                        CXX     = 'i586-mingw32msvc-c++',
                        AR      = 'i586-mingw32msvc-ar',
                        RANLIB  =  'i586-mingw32msvc-ranlib',
                        CPPPATH = [ '/home/ingo/projects/mingw32/run/include',
                                    '/home/ingo/projects/mingw32/run/opt/SDL-1.2.14/include/SDL',
                                    '/home/ingo/projects/mingw32/run/opt/SDL_image-1.2.10/include',
                                    '/home/ingo/projects/mingw32/run/opt/openal1.1/include',
                                    '/home/ingo/projects/mingw32/run/opt/gtk/include' ],
                        LIBPATH = [ '/home/ingo/projects/mingw32/run/lib',
                                    '/home/ingo/projects/mingw32/run/opt/SDL-1.2.14/lib',
                                    '/home/ingo/projects/mingw32/run/opt/SDL_image-1.2.10/lib',
                                    '/home/ingo/projects/mingw32/run/opt/gtk/lib' ],
                        PROGSUFFIX = '.exe')

            # gtk['ENV']['PATH'] = "/home/ingo/projects/mingw32/run/bin:" + gtk['ENV']['PATH']
            # gtk['ENV']['PKG_CONFIG_LIBDIR'] = "/home/ingo/projects/mingw32/run/lib/pkgconfig:/home/ingo/projects/mingw32/run/opt/gtk/lib/pkgconfig"

        if self.features['platform'] == 'darwin':
            cfg.Package('OpenAL', LINKFLAGS = [ '-framework', 'OpenAL' ])
            cfg.Package('OpenGL', LINKFLAGS = [ '-framework', 'OpenGL' ])
            cfg.Package('GLEW',   LIBS = [ 'GLEW' ])
            cfg.Package('SDL',
                        LINKFLAGS = [ '-framework', 'SDL',
                                      '-framework', 'Cocoa' ],
                        CPPPATH   = '/Library/Frameworks/SDL.framework/Headers/',
                        LIBS      = [ File('libsdlmain.a') ])
            cfg.Package('SDL_image',
                        LINKFLAGS = [ '-framework', 'SDL_image' ],
                        LIBS = [ 'jpeg' ],
                        CPPPATH = '/Library/Frameworks/SDL_image.framework/Headers/')
            cfg.Package('ogg',               CMD = "pkg-config ogg --cflags --libs")
            cfg.Package('vorbis',            CMD = "pkg-config vorbis --cflags --libs")
            cfg.Package('vorbisfile',        CMD = "pkg-config vorbisfile --cflags --libs")
            cfg.Package('boost_signals',     LIBS = [ 'boost_signals' ] )
            cfg.Package('boost_filesystem',  LIBS = [ 'boost_filesystem', 'boost_system' ] )
            cfg.Package('binreloc')           # not available on MacOSX
            cfg.Package('freetype',          CMD = "freetype-config --libs --cflags")
            cfg.Package('png',               CMD = "pkg-config --cflags --libs libpng")

        elif self.features['platform'] == 'win32':
            cfg.Package('OpenGL',           LIBS = [ 'opengl32', 'glu32' ])
            cfg.Package('OpenAL',           LIBS = [ 'openal32' ] )
            cfg.Package('GLEW',             LIBS = [ 'glew32' ] )
            cfg.Package('SDL',              LIBS = [ 'mingw32', 'SDLmain', 'SDL' ] )
            cfg.Package('SDL_image',        LIBS = [ 'SDL_image' ] )
            cfg.Package('ogg',              CMD = "pkg-config ogg --cflags --libs")
            cfg.Package('vorbis',           CMD = "pkg-config vorbis --cflags --libs")
            cfg.Package('vorbisfile',       CMD = "pkg-config vorbisfile --cflags --libs")
            cfg.Package('boost_signals',    LIBS = [ 'boost_signals' ] )
            cfg.Package('boost_filesystem', LIBS = [ 'boost_filesystem', 'boost_system' ] )
            cfg.Package('curl',             CMD = "pkg-config --cflags --libs libcurl | sed 's/-I/-isystem/g'")
            cfg.Package('png',              CMD = "pkg-config --cflags --libs libpng | sed 's/-I/-isystem/g'")
            cfg.Package('jpeg',             LIBS = [ 'jpeg' ] )
            cfg.Package('gtkmm-2.4',        CMD = "pkg-config --cflags --libs gtkmm-2.4 | sed 's/-I/-isystem/g'")
            cfg.Package('gtkglextmm-1.2',   CMD = "pkg-config --cflags --libs gtkglextmm-1.2 | sed 's/-I/-isystem/g'")
            cfg.Package('freetype',         CMD = "freetype-config --libs --cflags | sed 's/-I/-isystem/g'")
            cfg.Package('binreloc')         # not available on Win32

        else:
            cfg.Package('OpenGL',           LIBS = [ 'GL', 'GLU' ] )
            cfg.Package('OpenAL',           LIBS = [ 'openal' ] )
            cfg.Package('GLEW',             LIBS = [ 'GLEW' ] )
            cfg.Package('SDL',              CMD = "sdl-config --cflags --libs")
            cfg.Package('SDL_image',        LIBS = [ 'SDL_image' ], DEPENDS = [ 'SDL' ])
            cfg.Package('ogg',              CMD = "pkg-config ogg --cflags --libs")
            cfg.Package('vorbis',           CMD = "pkg-config vorbis --cflags --libs")
            cfg.Package('vorbisfile',       CMD = "pkg-config vorbisfile --cflags --libs")

            # FIXME: Add configure checks for exact boost library name (can be with or without -mt
            cfg.Package('boost_signals',    LIBS = [ 'boost_signals' ])
            cfg.Package('boost_filesystem', LIBS = [ 'boost_filesystem' ])

            cfg.Package('curl', CMD = "pkg-config --cflags --libs libcurl | sed 's/-I/-isystem/g'")
            cfg.Package('png',  CMD = "pkg-config --cflags --libs libpng | sed 's/-I/-isystem/g'")
            cfg.Package('jpeg',  LIBS = [ 'jpeg' ])
            cfg.Package('gtkmm-2.4',      CMD = "pkg-config --cflags --libs gtkmm-2.4 | sed 's/-I/-isystem/g'")
            cfg.Package('gtkglextmm-1.2', CMD = "pkg-config --cflags --libs gtkglextmm-1.2 | sed 's/-I/-isystem/g'")
            cfg.Package('freetype',   CMD = "freetype-config --libs --cflags | sed 's/-I/-isystem/g'")
            cfg.Package('binreloc',
                        CPPDEFINES = [ 'HAVE_BINRELOC' ],
                        CPPPATH    = [ 'external/binreloc-2.0/' ],
                        LIBS       = [ File('libbinreloc.a') ])

        cfg.Package('squirrel',
                    CPPPATH = [ 'external/SQUIRREL2/include/' ],
                    LIBS = [ File('libsquirrel.a') ],
                    CPPDEFINES = []) # empty, but it is needed later on

        cfg.Package('windstille', CPPPATH = [ '.', 'src', 'src/scripting' ])
        cfg.Package('test',  CPPDEFINES = [ '__TEST__' ],  OBJPREFIX = "test__" )

        cfg.Package('wst_display',   LIBS = [ File('libwst_display.a') ] )
        cfg.Package('wst_math',      LIBS = [ File('libwst_math.a') ] )
        cfg.Package('wst_navgraph',  LIBS = [ File('libwst_navgraph.a') ] )
        cfg.Package('wst_particles', LIBS = [ File('libwst_particles.a') ] )
        cfg.Package('wst_sound',     LIBS = [ File('libwst_sound.a') ] )
        cfg.Package('wst_system',    LIBS = [ File('libwst_system.a') ] )
        cfg.Package('wst_util',      LIBS = [ File('libwst_util.a') ] )

        if conf.Check32bit() == "64bit":
            # conf.env.Append(CXXFLAGS="-D_SQ64")
            self.features["64bit"] = True
            cfg['squirrel']['CPPDEFINES'] += ['_SQ64']

        if conf.CheckLib('cwiid'):
            self.features["cwiid"] = True

        if not conf.CheckLex():
            print "lex or flex not found, aborting."
            Exit(1)

        if not conf.CheckYacc():
            print "yacc or bison not found, aborting."
            Exit(1)

        self.env = conf.Finish()       

    def build_all(self):
        if self.features['platform'] == 'darwin':        
            self.build_sdl_main()
        elif self.features['platform'] == 'linux2':
            self.build_binreloc()  

        self.build_squirrel()
        self.build_miniswig()
        self.build_wstlib()
        self.build_windstille()

        self.build_windstille_editor()

        self.build_windstille_data()
        self.build_test_apps()
        self.build_extra_apps()

    def build_squirrel(self):
        pkg = { 'CPPPATH'  : [ 'external/SQUIRREL2/include' ],
                'CXXFLAGS' : [ "-fno-rtti", "-g", "-DDEBUG" ] }

        if self.features['64bit']:
            pkg['CPPDEFINES'] = ['_SQ64']

        BuildStaticLibrary('squirrel',
                           Glob('external/SQUIRREL2/squirrel/*.cpp') +
                           Glob('external/SQUIRREL2/sqstdlib/*.cpp'),
                           [pkg])

    def build_miniswig(self):
        miniswig_env = Environment()
        miniswig_env.Append(CPPPATH=['.', 'external/miniswig/'],
                            CXXFILESUFFIX = ".cpp",
                            YACCFLAGS=['-d', '--no-lines'])

        miniswig_bin = miniswig_env.Program('miniswig',
                                            ['external/miniswig/parser.yy',
                                             'external/miniswig/lexer.ll',
                                             'external/miniswig/create_docu.cpp',
                                             'external/miniswig/xmlwriter.cpp',
                                             'external/miniswig/create_wrapper.cpp',
                                             'external/miniswig/main.cpp',
                                             'external/miniswig/tree.cpp'])

        env = self.env.Clone()
        env.Append(MINISWIG=miniswig_bin)

        env.Depends(env.Command('src/scripting/miniswig.tmp', 'src/scripting/wrapper.interface.hpp',
                                ["$CXX -E -Isrc/ -x c -CC $SOURCE -o $TARGET -DSCRIPTING_API"]),
                    ['src/scripting/interface.hpp',
                     'src/scripting/game_objects.hpp'])

        env.Depends(env.Command(['src/scripting/wrapper.cpp', 'src/scripting/wrapper.hpp'],
                                'src/scripting/miniswig.tmp',
                                ["$MINISWIG --input $SOURCE --output-cpp ${TARGETS[0]} --output-hpp ${TARGETS[1]} "+
                                 "--module windstille --select-namespace Scripting"]),
                    miniswig_bin)

    def build_binreloc(self):
        BuildStaticLibrary("binreloc", ["external/binreloc-2.0/binreloc.c"],
                           [ { 'CPPDEFINES' : ["ENABLE_BINRELOC"] } ])

    def build_sdl_main(self):
        BuildStaticLibrary("sdlmain", ["src/macosx/SDLmain.m"], [ 'SDL' ])

    def build_wstlib(self):
        pkgs = [ 'default', 'windstille', 'binreloc' ]
        BuildStaticLibrary('wst_util',
                           Glob('src/lisp/*.cpp') +
                           Glob('src/util/*.cpp'),
                           pkgs)
        BuildStaticLibrary('wst_math', Glob('src/math/*.cpp'), pkgs)
        BuildStaticLibrary('wst_navgraph', Glob('src/navigation/*.cpp'), pkgs)
        BuildStaticLibrary('wst_particles', Glob('src/particles/*.cpp'), pkgs)
        BuildStaticLibrary('wst_sound', Glob('src/sound/*.cpp'), pkgs)
        BuildStaticLibrary('wst_display', 
                           Glob('src/font/*.cpp') +
                           Glob('src/display/*.cpp') +
                           Glob('src/scenegraph/*.cpp') +
                           Glob('src/sprite2d/*.cpp') +
                           Glob('src/sprite3d/*.cpp'),
                           pkgs + [ 'freetype', 'SDL', 'SDL_image' ])
        BuildStaticLibrary('wst_system', Glob('src/system/*.cpp'), pkgs + [ 'SDL' ])

    def build_windstille(self):
        BuildProgram('windstille',
                     Glob('src/app/*.cpp') +
                     Glob('src/armature/*.cpp') +
                     Glob('src/collision/*.cpp') +
                     Glob('src/engine/*.cpp') +
                     Glob('src/gui/*.cpp') +
                     Glob('src/hud/*.cpp') +
                     Glob('src/input/*.cpp') +
                     Glob('src/objects/*.cpp') +
                     Glob('src/properties/*.cpp') +
                     Glob('src/screen/*.cpp') +
                     Glob('src/scripting/*.cpp') +
                     Glob('src/tile/*.cpp'),
                     [ 'default', 'windstille',
                       'wst_particles', 'wst_navgraph', 'wst_display', 'wst_util', 'wst_math', 'wst_sound',
                       'freetype',
                       'SDL', 'SDL_image',
                       'OpenAL', 'ogg', 'vorbis', 'vorbisfile', 
                       'squirrel', 'png', 'jpeg', 'binreloc',
                       'OpenGL', 'GLEW',
                       'boost_signals', 'boost_filesystem' ])

    def build_windstille_editor(self):
        pkgs = [ 'default',
                 'windstille',
                 'wst_particles', 'wst_navgraph', 'wst_display', 'wst_util', 'wst_math', 'wst_sound',
                 'SDL', 'SDL_image',
                 'boost_filesystem',
                 'freetype',
                 'gtkglextmm-1.2' , 'gtkmm-2.4', 'SDL', 'curl', 'png', 'binreloc', 'OpenGL', 'GLEW',
                 'binreloc', 'jpeg' ]
        BuildProgram('windstille-editor', Glob('src/editor/*.cpp'), pkgs)

        # FIXME: temporary dirty hack
        # test_editor_env = editor_env.Clone(OBJPREFIX="test_")
        # test_editor_env.Append(CPPDEFINES=["__TEST__"])
        # test_editor_env.Program('test_animation_widget',
        #                        [f for f in Glob('src/editor/*.cpp') if f.get_path() != "src/editor/main.cpp"])

    def build_extra_apps(self):
        pkgs = [ 'default',
                 'windstille',
                 'wst_particles', 'wst_navgraph', 'wst_display', 'wst_math', 'wst_sound', 'wst_system', 'wst_util',
                 'SDL_image', 'OpenGL', 'GLEW', 'png', 'jpeg', 'boost_filesystem',
                 'OpenAL', 'ogg', 'vorbis', 'vorbisfile', 'SDL']

        BuildProgram("slideshow", Glob("extra/slideshow/*.cpp") + Glob("extra/slideshow/plugins/*.cpp"),
                     pkgs + [ { 'CPPPATH' : 'extra/' } ])
        BuildProgram("shadertest", Glob("extra/shadertest/*.cpp"), pkgs)
        BuildProgram("lensflare", Glob("extra/lensflare/*.cpp"), pkgs)
        BuildProgram("memleak", Glob("extra/memleak/*.cpp"), pkgs)
        BuildProgram("2dshadow", Glob("extra/2dshadow/*.cpp"), pkgs)

        for filename in Glob("extra/*.cpp", strings=True):
            BuildProgram(filename[:-4], filename, pkgs)

    def build_test_apps(self):
        pkgs = [ 'default', 'test', 'windstille' ]

        BuildProgram("test_babyxml", ["src/util/baby_xml.cpp"], pkgs)
        BuildProgram("test_response_curve", ["src/util/response_curve.cpp"], pkgs)
        BuildProgram("test_random", ["src/math/random.cpp"], pkgs)
        BuildProgram("test_pathname", ["src/util/pathname.cpp"], pkgs + [ 'boost_filesystem' ])
        BuildProgram("test_directory", ["src/util/directory.cpp"], pkgs + [ 'wst_util', 'boost_filesystem' ])
        BuildProgram("test_easing", ["src/math/easing.cpp"], pkgs)
        BuildProgram("reader_test", ["test/read_test.cpp"], pkgs + [ 'wst_util', 'SDL' ])

        BuildProgram("test_scissor_drawable", ["test/scissor_drawable/scissor_drawable.cpp"],
                     pkgs + [ 'wst_particles', 'wst_navgraph', 'wst_display', 'wst_math',
                              'wst_sound', 'wst_system', 'wst_util',
                              'SDL', 'OpenGL', 'GLEW', 'png', 'SDL_image', 'boost_filesystem', 'jpeg', 
                              'binreloc' ])

        BuildProgram("test_shader_drawable", [ "test/shader_drawable/shader_drawable.cpp" ],
                     pkgs + [ 'wst_particles', 'wst_navgraph', 'wst_display', 'wst_math',
                              'wst_sound', 'wst_system', 'wst_util',
                              'SDL', 'OpenGL', 'GLEW', 'png', 'SDL_image', 'boost_filesystem', 'jpeg',
                              'binreloc' ])

    def build_windstille_data(self):
        data_env = self.env.Clone()

        data_env.Append(BUILDERS = { "xcf2png" : Builder(action = "xcf2png $SOURCE -o $TARGET",
                                                         src_suffix = ".xcf",
                                                         suffix = ".png") })

        for filename in Glob("data/images/decal/*.xcf"):
            data_env.xcf2png(filename)

        data_files = ["data/sounds/*.wav",
                      "data/sounds/*.ogg",
                      "data/fonts/*.ttf",
                      "data/images/*.png",
                      "data/images/*/*.png",
                      "data/images/*/*/*.png"]

        for pattern in data_files:
            for filename in Glob(pattern):
                File(filename)

Alias('configure')

project = Project()
project.configure()
project.build_all()

print "Time reading SConscript: %.2fsec" % (time.time() - start_time)

# EOF #
