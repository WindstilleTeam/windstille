##  -*- python -*-
##  Windstille - A Sci-Fi Action-Adventure Game
##  Copyright (C) 2000,2005 Ingo Ruhnke <grumbel@gmx.de>
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

def MakeEnvironment(packages, cfg):
    env = Environment(ENV=os.environ)

    for pkg in packages:
        if type(pkg) == str:
            if pkg in cfg:
                env.Append(**cfg[pkg])
            else:
                raise Exception("Unknown package: %s" % pkg)
        elif type(pkg) == dict:
            env.Append(**pkg)
        else:
            raise Exception("Unknown package type: %s" % pkg)

    return env

def BuildProgram(target, sources, packages = [], cfg = {}):
    env = MakeEnvironment(packages, cfg)
    return env.Program(target, sources)

def BuildStaticLibrary(target, sources, packages = [], cfg = {}):
    env = MakeEnvironment(packages, cfg)
    return env.StaticLibrary(target, sources)

def ParseConfig(cmd):
    env = Environment()
    env['ENV']['PATH'] = os.environ['PATH']
    env.ParseConfig(cmd)

    cfg = {}

    for var in [ 'CCFLAGS', 'CPPDEFINES', 'CPPPATH', 'LIBS', 'LIBPATH' ]:
        if var in env and env[var] != []:
            cfg[var]  = env[var] 

    # if 'CCFLAGS' in cfg:
    #     cfg['CXXFLAGS'] = cfg['CCFLAGS']

    return cfg

class Project:
    def __init__(self):
        self.features = {
            "64bit" : False,
            "cwiid" : False
            }

        self.env = Environment(ENV=os.environ)

    def configure(self):
        # windstille_env.ParseConfig('sdl-config --cflags --libs | sed "s/-I/-isystem/g"')
        # windstille_env.ParseConfig('freetype-config --libs --cflags | sed "s/-I/-isystem/g"')

        #     }

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

        self.cfg = {}
        cfg = self.cfg

        # preset compiler flags
        cfg['release'] = { 'CCFLAGS' : [ "-O3", "-s" ] }
        cfg['profile'] = { 'CCFLAGS' : [ "-O2", "-g3", "-pg" ],
                           'LINKFLAGS' : [ "-pg" ] }
        cfg['debug']   = { 'CCFLAGS' : [ "-O0", "-g3" ] }
        cfg['development'] = { 'CCFLAGS' : [ "-O2", "-g3",
                                             "-ansi",
                                             "-pedantic",
                                             "-Wall",
                                             "-Wextra",
                                             "-Wnon-virtual-dtor",
                                             "-Weffc++",
                                             "-Wconversion",
                                             # "-Wold-style-cast",
                                             "-Werror",
                                             "-Wshadow",
                                             "-Wcast-qual",
                                             "-Winit-self", # only works with >= -O1
                                             "-Wno-unused-parameter",
                                             # "-Winline",
                                             # "-Wfloat-equal",
                                             # "-Wunreachable-code",
                                             ] }

        if 'BUILD' in self.env:
            print "Build Type: %s" % self.env['BUILD']
            cfg['default'] = cfg[self.env['BUILD']]
        else:
            print "Build Type: release"
            cfg['default'] = cfg['release']

        if sys.platform == 'darwin':
            cfg['OpenAL']    = { 'LINKFLAGS' : [ '-framework', 'OpenAL' ] }
            cfg['OpenGL']    = { 'LINKFLAGS' : [ '-framework', 'OpenGL' ] }
            cfg['GLEW']      = { 'LIBS' : [ 'GLEW' ] }
            cfg['SDL']       = { 'LINKFLAGS' : [ '-framework', 'SDL',
                                                 '-framework', 'Cocoa' ],
                                 'CPPPATH' : '/Library/Frameworks/SDL.framework/Headers/',
                                 'LIBS'    : 'src/macosx/SDLmain.m' }
            cfg['SDL_image'] = { 'LINKFLAGS' : [ '-framework', 'SDL_image' ],
                                 'LIBS' : [ 'jpeg' ],
                                 'CPPPATH' : '/Library/Frameworks/SDL_image.framework/Headers/' }
            cfg['ogg']       = ParseConfig("pkg-config ogg --cflags --libs")
            cfg['vorbis']    = ParseConfig("pkg-config vorbis --cflags --libs")
            cfg['vorbisfile']       = ParseConfig("pkg-config vorbisfile --cflags --libs")            
            cfg['boost_signals']    = { 'LIBS' : [ 'boost_signals' ] }
            cfg['boost_filesystem'] = { 'LIBS' : [ 'boost_filesystem', 'boost_system' ] }
            cfg['binreloc']         = { } # not available on MacOSX
            cfg['freetype']         = ParseConfig("freetype-config --libs --cflags | sed 's/-I/-isystem/g'")
            cfg['png']              = ParseConfig("pkg-config --cflags --libs libpng | sed 's/-I/-isystem/g'")

        else:
            cfg['OpenGL']           = { 'LIBS' : [ 'GL', 'GLU' ] }
            cfg['OpenAL']           = { 'LIBS' : [ 'openal' ] }
            cfg['GLEW']             = { 'LIBS' : [ 'GLEW' ] }
            cfg['SDL']              = ParseConfig("sdl-config --cflags --libs")
            cfg['SDL_image']        = { 'LIBS' : [ 'SDL_image' ] }
            cfg['ogg']              = ParseConfig("pkg-config ogg --cflags --libs")
            cfg['vorbis']           = ParseConfig("pkg-config vorbis --cflags --libs")
            cfg['vorbisfile']       = ParseConfig("pkg-config vorbisfile --cflags --libs")

            # FIXME: Add configure checks for exact boost library name (can be with or without -mt
            cfg['boost_signals']    = { 'LIBS' : [ 'boost_signals' ] }
            cfg['boost_filesystem'] = { 'LIBS' : [ 'boost_filesystem' ] }

            cfg['curl']             = ParseConfig("pkg-config --cflags --libs libcurl | sed 's/-I/-isystem/g'")
            cfg['png']              = ParseConfig("pkg-config --cflags --libs libpng | sed 's/-I/-isystem/g'")
            cfg['jpeg']             = { 'LIBS' : [ 'jpeg' ] }
            cfg['gtkmm-2.4']        = ParseConfig("pkg-config --cflags --libs gtkmm-2.4 | sed 's/-I/-isystem/g'")
            cfg['gtkglextmm-1.2']   = ParseConfig("pkg-config --cflags --libs gtkglextmm-1.2 | sed 's/-I/-isystem/g'")
            cfg['freetype']         = ParseConfig("freetype-config --libs --cflags | sed 's/-I/-isystem/g'")
            cfg['binreloc']         = { 'CPPDEFINES' : [ 'HAVE_BINRELOC' ],
                                        'CPPPATH' : [ 'external/binreloc-2.0/' ],
                                        'LIBS' : [ File('libbinreloc.a') ] }

        cfg['squirrel']         = { 'CPPPATH' : [ 'external/SQUIRREL2/include/' ],
                                    'LIBS' : [ File('libsquirrel.a') ],
                                    'CPPDEFINES' : [] # empty, but it is needed later on
                                    }
        
        cfg['windstille']    = { 'CPPPATH' : [ '.', 'src', 'src/scripting' ] }
        cfg['test']          = { 'CPPDEFINES' : [ '__TEST__' ],
                                 'OBJPREFIX'  : "test__" }

        cfg['wst_display']   = { 'LIBS' : [ File('libwst_display.a') ] }
        cfg['wst_math']      = { 'LIBS' : [ File('libwst_math.a') ] }
        cfg['wst_navgraph']  = { 'LIBS' : [ File('libwst_navgraph.a') ] }
        cfg['wst_particles'] = { 'LIBS' : [ File('libwst_particles.a') ] }
        cfg['wst_sound']     = { 'LIBS' : [ File('libwst_sound.a') ] }
        cfg['wst_system']    = { 'LIBS' : [ File('libwst_system.a') ] }
        cfg['wst_util']      = { 'LIBS' : [ File('libwst_util.a') ] }

        if False:
            print
            print "Current Configuration:"
            print "======================"
            for k, v in self.cfg.items():
                print "cfg['%s'] = %s" % (k, v)
            print

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
        self.build_squirrel()
        self.build_miniswig()
        self.build_binreloc()
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
                           [pkg], self.cfg)

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
                           [ { 'CPPDEFINES' : ["ENABLE_BINRELOC"] } ], self.cfg)

    def build_wstlib(self):
        pkgs = [ 'default', 'windstille', 'binreloc' ]
        BuildStaticLibrary('wst_util',
                           Glob('src/lisp/*.cpp') +
                           Glob('src/util/*.cpp'),
                           pkgs + [ 'SDL' ], self.cfg)
        BuildStaticLibrary('wst_math', Glob('src/math/*.cpp'), pkgs, self.cfg)
        BuildStaticLibrary('wst_navgraph', Glob('src/navigation/*.cpp'), pkgs, self.cfg)
        BuildStaticLibrary('wst_particles', Glob('src/particles/*.cpp'), pkgs, self.cfg)
        BuildStaticLibrary('wst_sound', Glob('src/sound/*.cpp'), pkgs, self.cfg)
        BuildStaticLibrary('wst_display', 
                           Glob('src/font/*.cpp') +
                           Glob('src/display/*.cpp') +
                           Glob('src/scenegraph/*.cpp') +
                           Glob('src/sprite2d/*.cpp') +
                           Glob('src/sprite3d/*.cpp'),
                           pkgs + [ 'freetype', 'SDL', 'SDL_image' ], self.cfg)
        BuildStaticLibrary('wst_system', Glob('src/system/*.cpp'), pkgs + [ 'SDL' ], self.cfg)

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
                       'OpenGL', 'GLEW',
                       'freetype',
                       'SDL', 'SDL_image',
                       'OpenAL', 'ogg', 'vorbis', 'vorbisfile', 
                       'squirrel', 'png', 'binreloc',
                       'boost_signals', 'boost_filesystem' ],
                     self.cfg)

    def build_windstille_editor(self):
        pkgs = [ 'default',
                 'windstille',
                 'wst_particles', 'wst_navgraph', 'wst_display', 'wst_util', 'wst_math', 'wst_sound',
                 'SDL', 'SDL_image',
                 'boost_filesystem',
                 'gtkglextmm-1.2' , 'gtkmm-2.4', 'SDL', 'curl', 'png', 'binreloc', 'OpenGL', 'GLEW',
                 'binreloc', 'jpeg' ]
        BuildProgram('windstille-editor', Glob('src/editor/*.cpp'), pkgs, self.cfg)

        # FIXME: temporary dirty hack
        # test_editor_env = editor_env.Clone(OBJPREFIX="test_")
        # test_editor_env.Append(CPPDEFINES=["__TEST__"])
        # test_editor_env.Program('test_animation_widget',
        #                        [f for f in Glob('src/editor/*.cpp') if f.get_path() != "src/editor/main.cpp"])

    def build_extra_apps(self):
        pkgs = [ 'default',
                 'windstille',
                 'wst_particles', 'wst_navgraph', 'wst_display', 'wst_math', 'wst_sound', 'wst_system', 'wst_util',
                 'SDL', 'SDL_image', 'OpenGL', 'GLEW', 'png', 'boost_filesystem',
                 'OpenAL', 'ogg', 'vorbis', 'vorbisfile']

        BuildProgram("slideshow", Glob("extra/slideshow/*.cpp") + Glob("extra/slideshow/plugins/*.cpp"),
                     pkgs + [ { 'CPPPATH' : 'extra/' } ], self.cfg)
        BuildProgram("shadertest", Glob("extra/shadertest/*.cpp"), pkgs, self.cfg)
        BuildProgram("lensflare", Glob("extra/lensflare/*.cpp"), pkgs, self.cfg)
        BuildProgram("memleak", Glob("extra/memleak/*.cpp"), pkgs, self.cfg)

        for filename in Glob("extra/*.cpp", strings=True):
            BuildProgram(filename[:-4], filename, pkgs, self.cfg)

    def build_test_apps(self):
        pkgs = [ 'test', 'windstille' ]

        BuildProgram("test_babyxml", ["src/util/baby_xml.cpp"], pkgs, self.cfg)
        BuildProgram("test_response_curve", ["src/util/response_curve.cpp"], pkgs, self.cfg)
        BuildProgram("test_random", ["src/math/random.cpp"], pkgs, self.cfg)
        BuildProgram("test_pathname", ["src/util/pathname.cpp"], pkgs + [ 'boost_filesystem' ], self.cfg)
        BuildProgram("test_directory", ["src/util/directory.cpp"], pkgs + [ 'boost_filesystem', 'wst_util' ], self.cfg)
        BuildProgram("test_easing", ["src/math/easing.cpp"], pkgs, self.cfg)

        BuildProgram("test_scissor_drawable", ["test/scissor_drawable/scissor_drawable.cpp"],
                     pkgs + [ 'SDL', 'OpenGL', 'GLEW', 'png', 'SDL_image', 'boost_filesystem', 
                              'wst_particles', 'wst_navgraph', 'wst_display', 'wst_math', 'wst_sound', 'wst_system', 'wst_util',
                              'binreloc' ],
                     self.cfg)

        BuildProgram("test_shader_drawable", [ "test/shader_drawable/shader_drawable.cpp" ],
                     pkgs + [ 'SDL', 'OpenGL', 'GLEW', 'png', 'SDL_image', 'boost_filesystem', 
                              'wst_particles', 'wst_navgraph', 'wst_display', 'wst_math', 'wst_sound', 'wst_system', 'wst_util',
                              'binreloc' ],
                     self.cfg)

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

if 'configure' in COMMAND_LINE_TARGETS or \
   not (File('config.py').exists() and not GetOption('clean')):
    project.configure()
    project.build_all()
else:
    project.build_all()

print "Time reading SConscript: %.2fsec" % (time.time() - start_time)

# EOF #
