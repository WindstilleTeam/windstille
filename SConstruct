##  -*- python -*-
##  $Id: windstille_main.hpp 1097 2005-09-23 16:01:59Z grumbel $
##   __      __ __             ___        __   __ __   __
##  /  \    /  \__| ____    __| _/_______/  |_|__|  | |  |   ____
##  \   \/\/   /  |/    \  / __ |/  ___/\   __\  |  | |  | _/ __ \
##   \        /|  |   |  \/ /_/ |\___ \  |  | |  |  |_|  |_\  ___/
##    \__/\  / |__|___|  /\____ /____  > |__| |__|____/____/\___  >
##         \/          \/      \/    \/                         \/
##  Copyright (C) 2000,2005 Ingo Ruhnke <grumbel@gmx.de>
##
##  This program is free software; you can redistribute it and/or
##  modify it under the terms of the GNU General Public License
##  as published by the Free Software Foundation; either version 2
##  of the License, or (at your option) any later version.
##
##  This program is distributed in the hope that it will be useful,
##  but WITHOUT ANY WARRANTY; without even the implied warranty of
##  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
##  GNU General Public License for more details.
## 
##  You should have received a copy of the GNU General Public License
##  along with this program; if not, write to the Free Software
##  Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA
##  02111-1307, USA.
##

import sys, os
sys.path.append('scons')

def CheckSDL(context, version):
    context.Message( 'Checking for SDL >= %s ...' % version )
    (ret, outstring) = context.TryAction('sdl-config --version')
    if ret:
        outstring = outstring[0:-1]
        if outstring != version:
            context.Result( "failed, got '%s' instead" % outstring )
            return 0
        else:
            context.Result( 1 )
            return 1
    else:
        context.Result( ret )
        return ret

def CheckPhysFS(context, version):
    optionFile = env['CACHEDIR'] + 'libphysfs.cache.py'
    opts = Options(optionFile)
    opts.AddOptions(
        ('CACHED_LIBPHYSFS',   'Whether libphysfs is available'),
        ('CXXFLAGS_LIBPHYSFS', ''),
        ('LINKFLAGS_LIBPHYSFS',''),
        )
    opts.Update(context.env)

    context.Message( 'Checking for PhysFS >= %s ...' % version )
    if not context.env.has_key('CACHED_LIBPHYSFS'):
        oldLIBS = context.env.get( 'LIBS', [] )
        context.env.Append(LIBS = [ 'physfs' ])
        ret = context.TryCompile("""
        #include <physfs.h>
        int main()
        {
           return 0;
        }
        """, '.c')
        oldLIBS = context.env.get( 'LIBS', [] )
        context.env.Replace(LIBS = oldLIBS)

        if ret:
            env['CACHED_LIBPHYSFS']  = '1'
            env['CXXFLAGS_LIBPHYSFS']  = ''
            env['LINKFLAGS_LIBPHYSFS'] = '-lphysfs'

            opts.Save(optionFile, context.env)
            context.Result(ret)
            return ret
        else:
            context.Result(ret)
        return ret
    else:
        context.Result('ok (cached)')
        return 1

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

opts = Options(['options.cache', 'custom.py'], ARGUMENTS)
opts.Add('CPPPATH', 'Additional preprocessor paths')
opts.Add('CPPFLAGS', 'Additional preprocessor flags')
opts.Add('CPPDEFINES', 'defined constants')
opts.Add('LIBPATH', 'Additional library paths')
opts.Add('LIBS', 'Additional libraries')
opts.Add('CCFLAGS', 'C Compiler flags')
opts.Add('LINKFLAGS', 'Linker Compiler flags')
opts.Add('CC', 'C Compiler')
opts.Add('CXX', 'C++ Compiler')  
opts.Update(conf_env)
opts.Save('options.cache', conf_env)
Help(opts.GenerateHelpText(conf_env))

conf = Configure(conf_env, custom_tests = { 'Check32bit' : Check32bit,
                                            'CheckYacc'  : CheckYacc,
                                            'CheckLex'   : CheckLex})
if conf.Check32bit() == "64bit":
  conf.env.Append(CXXFLAGS="-D_SQ64")

if not conf.CheckLex():
    print "lex or flex not found, aborting."
    Exit(1)
    
if not conf.CheckYacc():
    print "yacc or bison not found, aborting."
    Exit(1)
    
conf_env = conf.Finish()

Export('conf_env')

# SConscript('build/tools/SConscript')
# SConscript('build/lib/SConscript')
# SConscript('build/src/SConscript')
# SConscript('build/src/scripting/SConscript')

SConscript('tools/SConscript')
SConscript('tools/sdl-jstest/SConscript')
SConscript('lib/SConscript')
SConscript('src/SConscript')

# EOF #
