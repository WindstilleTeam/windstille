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

import sys, os, glob
sys.path.append('scons')

CacheDir('cache')

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


# FIXME: Add check for ICONV_CONST

# BuildDir('build/src',   'src', duplicate=0)
# BuildDir('build/lib',   'lib', duplicate=0)
# BuildDir('build/tools', 'tools', duplicate=0)

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

features = {
    "64bit" : 0,
    "cwiid" : 0
    }

conf = Configure(conf_env, custom_tests = { 'Check32bit' : Check32bit,
                                            'CheckYacc'  : CheckYacc,
                                            'CheckLex'   : CheckLex})
if conf.Check32bit() == "64bit":
    # conf.env.Append(CXXFLAGS="-D_SQ64")
    features["64bit"] = 1

if conf.CheckLib('cwiid'):
    features["cwiid"] = 1
    
if not conf.CheckLex():
    print "lex or flex not found, aborting."
    Exit(1)
    
if not conf.CheckYacc():
    print "yacc or bison not found, aborting."
    Exit(1)
    
conf_env = conf.Finish()

# SConscript('build/tools/SConscript')
# SConscript('build/lib/SConscript')
# SConscript('build/src/SConscript')
# SConscript('build/src/scripting/SConscript')

squirrel_env = Environment(CPPPATH  = ['external/SQUIRREL2/include'],
                           CXXFLAGS = ["-fno-rtti", "-g", "-DDEBUG"])

if features['64bit']:
    squirrel_env.Append(CPPDEFINES = '_SQ64')

# squirrel_env.Append(CXXFLAGS = conf_env['CXXFLAGS'])

squirrel_env.Library('squirrel',
                     Glob('external/SQUIRREL2/squirrel/*.cpp') +
                     Glob('external/SQUIRREL2/sqstdlib/*.cpp'))


miniswig_env = Environment(CPPPATH=['.', "..", "miniswig"],
                           CXXFILESUFFIX = ".cpp",
                           YACCFLAGS=['-d', '--no-lines'])

miniswig = miniswig_env.Program('external/miniswig/miniswig',
                                ['external/miniswig/parser.yy',
                                 'external/miniswig/lexer.ll',
                                 'external/miniswig/create_docu.cpp',
                                 'external/miniswig/xmlwriter.cpp',
                                 'external/miniswig/create_wrapper.cpp',
                                 'external/miniswig/main.cpp',
                                 'external/miniswig/tree.cpp'])

env = Environment(MINISWIG='external/miniswig/miniswig')

env.Depends(env.Command('src/scripting/miniswig.tmp', 'src/scripting/wrapper.interface.hpp',
                        ["cpp -x c  -CC $SOURCE -o $TARGET -DSCRIPTING_API"]),
            ['src/scripting/interface.hpp',
             'src/scripting/game_objects.hpp'])

env.Depends(env.Command(['src/scripting/wrapper.cpp', 'src/scripting/wrapper.hpp'], 'src/scripting/miniswig.tmp',
                        ["$MINISWIG  --input $SOURCE --output-cpp ${TARGETS[0]} --output-hpp ${TARGETS[1]} --module windstille --select-namespace Scripting"]),
            miniswig)

Export('conf_env', 'features', 'miniswig')

def expand_path(*args):
    lst = []
    for i in args:
        lst += Glob(i)
    return lst

# libutil
util_env = conf_env.Clone()
util_env.Append(CPPPATH=['src'])
util_lib = util_env.StaticLibrary('util', expand_path('src/lisp/*.cpp',  'src/util/*.cpp'))

# libphysfs
physfs_env = conf_env.Clone()
physfs_env.Append(CPPPATH=['src'])
physfs_env.ParseConfig('sdl-config --cflags --libs')
physfs_lib = physfs_env.StaticLibrary('src/physfs', expand_path('src/physfs/*.cpp'))

# libmath
math_env = conf_env.Clone()
math_env.Append(CPPPATH=['src'])
math_lib = math_env.StaticLibrary('math', expand_path('src/math/*.cpp'))

# libnavgraph
navgraph_env = conf_env.Clone()
navgraph_env.Append(CPPPATH=['src'])
navgraph_lib = navgraph_env.StaticLibrary('navgraph', expand_path('src/navigation/*.cpp'))

# libparticles
particles_env = conf_env.Clone()
particles_env.Append(CPPPATH=['src'])
particles_lib = particles_env.StaticLibrary('particles', expand_path('src/particles/*.cpp'))

# libdisplay
display_env = conf_env.Clone()
display_env.Append(CPPPATH=['src'])
display_env.ParseConfig('sdl-config --cflags --libs')
display_env.ParseConfig('freetype-config --libs --cflags')
display_lib = display_env.StaticLibrary('display', 
                                        expand_path('src/font/*.cpp',
                                                    'src/display/*.cpp',
                                                    'src/sprite2d/*.cpp',
                                                    'src/sprite3d/*.cpp'))


env = conf_env.Clone()
env.ParseConfig('sdl-config --cflags --libs')
env.ParseConfig('freetype-config --libs --cflags')
env.Append(LIBS=[particles_lib, navgraph_lib, display_lib, physfs_lib, util_lib, math_lib,
                 'GL', 'GLU', 'GLEW',
                 'squirrel', 'physfs', 'SDL_image', 'openal', 'ogg', 'vorbis', 'vorbisfile', 'png', 'boost_signals-mt'])
env.Append(LIBPATH=['external/'])
env.Append(CPPPATH=['src', '.', 'external/SQUIRREL2/include/'])

if features['64bit']:
    env.Append(CPPDEFINES = '_SQ64')

if features['cwiid']:
    env.Append(CPPDEFINES = 'HAVE_CWIID')
    env.Append(LIBS = 'cwiid')

env.Program('windstille', expand_path('src/app/*.cpp',
                                         'src/armature/*.cpp',
                                         'src/collision/*.cpp',
                                         'src/engine/*.cpp',
                                         'src/gui/*.cpp',
                                         'src/hud/*.cpp',
                                         'src/input/*.cpp',
                                         'src/objects/*.cpp',
                                         'src/properties/*.cpp',
                                         'src/screen/*.cpp',
                                         'src/scripting/*.cpp',
                                         'src/sound/*.cpp',
                                         'src/tile/*.cpp'))

debug_cxxflags = ["-O0", "-g", "-Wall", "-ansi", "-pedantic", "-Wnon-virtual-dtor", "-Werror"] # "-Wconversion",
# -W: unused parameter
# ['-Wall', '-Werror', '-O2', '-g'],
editor_env = Environment(CPPPATH=['.'],
                         CXXFLAGS=debug_cxxflags,
                         LIBS = [particles_lib, navgraph_lib, display_lib, util_lib, physfs_lib, math_lib,
                                 'GL', 'GLEW', 'SDL_image', 'physfs'])

editor_env.ParseConfig('pkg-config libpng --libs --cflags')
editor_env.ParseConfig('sdl-config --cflags --libs')
editor_env.ParseConfig('Magick++-config --libs --cppflags')
editor_env.ParseConfig('pkg-config --cflags --libs libcurl')
editor_env.ParseConfig('pkg-config --cflags --libs gtkmm-2.4 gtkglextmm-1.2')

editor_env.Program('../windstille-editor', expand_path('editor/*.cpp'))

data_env = conf_env.Clone()

data_env.Append(BUILDERS = { "xcf2png" : Builder(action = "xcf2png $SOURCE -o $TARGET",
                                                 src_suffix = ".xcf",
                                                 suffix = ".png") })

for filename in glob.glob("data/images/decal/*.xcf"):
    data_env.xcf2png(filename)

# EOF #
