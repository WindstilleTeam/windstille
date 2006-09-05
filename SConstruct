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
    (ret, outstring) = context.TryAction('sdl-config --version > $TARGET')
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

opts = Options(['options.cache', 'custom.py'])
opts.Add('CXX', 'The C++ compiler')

env = Environment(options=opts)

env["CACHEDIR"] = "cache/"
if not os.path.isdir(env['CACHEDIR']): os.mkdir(env['CACHEDIR'])

# env.BuildDir('build2', '.')

if 'configure' in COMMAND_LINE_TARGETS:
    conf = Configure( env, custom_tests = { 'CheckSDL' : CheckSDL, 'CheckPhysFS' : CheckPhysFS } )

    if not conf.CheckSDL('1.2.8'):
        print 'We really need SDL!'
        Exit(1)
        
    if not conf.CheckPhysFS('1.0'):
        print "libphysfs missing"
        Exit(1)

    env = conf.Finish() 
else:
    Export('env')
    SConscript('tools/SConscript')
    SConscript('lib//SConscript')
    SConscript('src/SConscript')
    SConscript('src/scripting/SConscript')

# EOF #
