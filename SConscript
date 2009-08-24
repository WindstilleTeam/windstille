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

start_time = time.time()

CacheDir('cache')

debug_cxxflags = [
    "-O0",
    "-g",
    "-ansi",
    "-pedantic",
    "-Wall",
    "-Wextra",
    "-Wnon-virtual-dtor",
    "-Weffc++",
    "-Wconversion",
    "-Werror",
    #"-Wshadow",
    #"-Wunreachable-code",
    #"-Winline",
    ]

windstille_cxxflags = debug_cxxflags

# YACC
yacc_test_text = """
%{
#include <stdio.h>

/* MSVC++ needs this before it can swallow Bison output */
#ifdef _MSC_VER
# define __STDC__
#endif
%}
%token MSG
%start ROOT
%%
ROOT:
    MSG { printf("HELLO"); } 
    ;
%%
"""

def CheckYacc(context):
    context.Message("Checking for Yacc ('%s')... " % context.env.get('YACC'))
    is_ok = context.TryCompile(yacc_test_text,".y")
    context.Result(is_ok)
    return is_ok

# LEX
lex_test_text = """
%{
#include <stdio.h>
%}
DIGIT	[0-9]
ID		[a-z][a-z0-9]*
%%
{DIGIT}+	{
		printf("A digit: %s\\n",yytext);
	}

[ \\t\\n]+    /* ignore */

.			{
		printf("Unrecognized guff");
	}
%%
main(){
	yylex();
}
"""

def CheckLex(context):
    context.Message("Checking for Lex ('%s')... " % context.env.get('LEX'))
    is_ok = context.TryCompile(lex_test_text,".l")
    context.Result(is_ok)
    return is_ok


def Check32bit(context):
    check32bit_test_source_file = """
#include <stdio.h>
int main()
{
   printf("%dbit", sizeof(void*)*8);
   return 0;
}
    """
    context.Message('Checking for bits... ')
    (suc, output) = context.TryRun(check32bit_test_source_file, '.cpp')
    if suc:
        context.Result(output)
    else:
        context.Result("test error")
    return output


class Project:
    def __init__(self):
        self.features = {
            "64bit" : 0,
            "cwiid" : 0
            }        

    def build_all(self):
        self.configure()
        self.build_squirrel()
        self.build_miniswig()
        self.build_binreloc()
        self.build_wstlib()
        self.build_windstille()
        self.build_windstille_editor()
        self.build_windstille_data()
        self.build_testapps()
        
    def configure(self):
        conf_env = Environment()

        opts = Variables(['options.cache', 'custom.py'], ARGUMENTS)
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
        opts.Update(conf_env)
        opts.Save('options.cache', conf_env)
        Help(opts.GenerateHelpText(conf_env))

        # FIXME: Giving multiple CCFLAGS doesn't work since they have to be
        # broken down to a list

        conf = Configure(conf_env, custom_tests = { 'Check32bit' : Check32bit,
                                                    'CheckYacc'  : CheckYacc,
                                                    'CheckLex'   : CheckLex})
        if conf.Check32bit() == "64bit":
            # conf.env.Append(CXXFLAGS="-D_SQ64")
            self.features["64bit"] = 1

        if conf.CheckLib('cwiid'):
            self.features["cwiid"] = 1

        if not conf.CheckLex():
            print "lex or flex not found, aborting."
            Exit(1)

        if not conf.CheckYacc():
            print "yacc or bison not found, aborting."
            Exit(1)

        conf_env = conf.Finish()

    def build_squirrel(self):
        squirrel_env = Environment(CPPPATH  = ['external/SQUIRREL2/include'],
                                   CXXFLAGS = ["-fno-rtti", "-g", "-DDEBUG"])

        if self.features['64bit']:
            squirrel_env.Append(CPPDEFINES = '_SQ64')

        self.squirrel_lib = squirrel_env.StaticLibrary('squirrel',
                                                       Glob('external/SQUIRREL2/squirrel/*.cpp') +
                                                       Glob('external/SQUIRREL2/sqstdlib/*.cpp'))


    def build_miniswig(self):
        miniswig_env = Environment(CPPPATH=['.', 'external/miniswig/'],
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

        env = Environment(MINISWIG=miniswig_bin)

        env.Depends(env.Command('src/scripting/miniswig.tmp', 'src/scripting/wrapper.interface.hpp',
                                ["$CXX -E -Isrc/ -x c -CC $SOURCE -o $TARGET -DSCRIPTING_API"]),
                    ['src/scripting/interface.hpp',
                     'src/scripting/game_objects.hpp'])

        env.Depends(env.Command(['src/scripting/wrapper.cpp', 'src/scripting/wrapper.hpp'], 'src/scripting/miniswig.tmp',
                                ["$MINISWIG --input $SOURCE --output-cpp ${TARGETS[0]} --output-hpp ${TARGETS[1]} "+
                                 "--module windstille --select-namespace Scripting"]),
                    miniswig_bin)

    def build_binreloc(self):
        self.binreloc_lib = StaticLibrary("binreloc", ["external/binreloc-2.0/binreloc.c"], CPPDEFINES=["ENABLE_BINRELOC"])

    def build_wstlib(self):
        wstlib_env    = Environment(CPPPATH=['src', 'external/binreloc-2.0/', "src/scripting/"],
                                    CXXFLAGS=windstille_cxxflags,
                                    CPPDEFINES=["HAVE_BINRELOC"])
        self.util_lib      = wstlib_env.StaticLibrary('util', Glob('src/lisp/*.cpp') + Glob('src/util/*.cpp'))
        self.math_lib      = wstlib_env.StaticLibrary('math', Glob('src/math/*.cpp'))
        self.navgraph_lib  = wstlib_env.StaticLibrary('navgraph', Glob('src/navigation/*.cpp'))
        self.particles_lib = wstlib_env.StaticLibrary('particles', Glob('src/particles/*.cpp'))
        
        # libphysfs
        physfs_env = wstlib_env.Clone()
        physfs_env.ParseConfig('sdl-config --cflags --libs | sed "s/-I/-isystem/g"')
        self.physfs_lib = physfs_env.StaticLibrary('src/physfs', Glob('src/physfs/*.cpp'))

        # libdisplay
        display_env = wstlib_env.Clone()
        display_env.ParseConfig('sdl-config --cflags --libs | sed "s/-I/-isystem/g"')
        display_env.ParseConfig('freetype-config --libs --cflags | sed "s/-I/-isystem/g"')
        self.display_lib = display_env.StaticLibrary('display', 
                                                     Glob('src/font/*.cpp') +
                                                     Glob('src/display/*.cpp') +
                                                     Glob('src/scenegraph/*.cpp') +
                                                     Glob('src/sprite2d/*.cpp') +
                                                     Glob('src/sprite3d/*.cpp'))

    def build_windstille(self):
        windstille_env = Environment(CXXFLAGS=windstille_cxxflags,
                                     CPPPATH=['src', '.', 'external/SQUIRREL2/include/', 'src/scripting/'],
                                     CPPDEFINES=["HAVE_BINRELOC"],
                                     LIBS=[self.particles_lib, self.navgraph_lib, self.display_lib, self.physfs_lib,
                                           self.util_lib, self.math_lib,
                                           self.binreloc_lib, self.squirrel_lib, 
                                           'GL', 'GLU', 'GLEW',
                                           'physfs', 'SDL_image', 'openal', 'ogg', 'vorbis', 'vorbisfile', 'png',
                                           # FIXME: Add configure checks for exact boost library name
                                           'boost_signals-mt', 'boost_filesystem-mt'])

        windstille_env.ParseConfig('sdl-config --cflags --libs | sed "s/-I/-isystem/g"')
        windstille_env.ParseConfig('freetype-config --libs --cflags | sed "s/-I/-isystem/g"')

        if self.features['64bit']:
            windstille_env.Append(CPPDEFINES = '_SQ64')

        if self.features['cwiid']:
            windstille_env.Append(CPPDEFINES = 'HAVE_CWIID')
            windstille_env.Append(LIBS = 'cwiid')

        windstille_env.Program('windstille',
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
                               Glob('src/sound/*.cpp') +
                               Glob('src/tile/*.cpp'))

    def build_windstille_editor(self):
        editor_env = Environment(CPPPATH=['src'],
                                 CPPDEFINES=["HAVE_BINRELOC"],
                                 CXXFLAGS=windstille_cxxflags,
                                 LIBS = [self.particles_lib, self.navgraph_lib, self.display_lib,
                                         self.util_lib, self.physfs_lib, self.math_lib, self.binreloc_lib,
                                         'GL', 'GLEW', 'SDL_image', 'physfs', 'boost_filesystem-mt'])

        editor_env.ParseConfig('Magick++-config --libs --cppflags | sed "s/-I/-isystem/g"')
        editor_env.ParseConfig('sdl-config --cflags --libs | sed "s/-I/-isystem/g"')
        editor_env.ParseConfig('pkg-config --cflags --libs libcurl | sed "s/-I/-isystem/g"')
        editor_env.ParseConfig('pkg-config --cflags --libs libpng | sed "s/-I/-isystem/g"')

        # Turn -I options into -isystem, so we don't get warnings from external libraries
        editor_env.ParseConfig('pkg-config --cflags --libs gtkmm-2.4 | sed "s/-I/-isystem/g"')
        editor_env.ParseConfig('pkg-config --cflags --libs gtkglextmm-1.2 | sed "s/-I/-isystem/g"')

        editor_env.Program('windstille-editor', Glob('src/editor/*.cpp'))

    def build_testapps(self):
        env = Environment(OBJPREFIX="test__",
                          CPPPATH=['src'],
                          CXXFLAGS=windstille_cxxflags,
                          CPPDEFINES=["__TEST__"])
        env.Program("test_babyxml", ["src/util/baby_xml.cpp"])
        env.Program("test_response_curve", ["src/util/response_curve.cpp"])
        env.Program("test_random", ["src/math/random.cpp"])

    def build_windstille_data(self):
        data_env = Environment()

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

project = Project()
project.build_all()

print "Time reading SConscript: %.2fsec" % (time.time() - start_time)

# EOF #
