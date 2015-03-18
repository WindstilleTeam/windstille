Windstille Compilation
======================

Hardware Requirements
---------------------

Minimal requirements at which the game will run, albeit not very
smoothly:

 * Geforce2MX
 * 1GHZ Athlon

The game will not work with a MatroxG450 and hasn't been tested with
ATI cards.


Requirements
------------

To compile Windstille you need:

 * OpenAL
 * OpenGL
 * SDL2
 * SDL2_image
 * freetype2
 * libpng
 * physfs
 * boost
 * scons
 * bison (bison++ will not work)
 * flex
 * gtkmm (for the editor only)
 * gtkglextmm (for the editor only)


### Linux

In most cases these will come with your distribution. In Ubuntu 15.04
you can install them with:

    $ sudo apt-get install \
        bison \
        flex \
        libboost-dev \
        libboost-filesystem-dev \
        libboost-signals-dev \
        libboost-thread-dev \
        libfreetype6-dev \
        libphysfs-dev \
        libsdl2-image-dev \
        libsdl2-dev \
        scons \
        libpng-dev \
        libopenal-dev \
        libgtkmm-2.4-dev \
        libgtkglextmm-x11-1.2-dev \
        libmagick++-dev \
        libcurl4-gnutls-dev \
        xcftools \
        libglew1.5-dev \
        libogg-dev \
        libvorbis-dev


### Mac OS X

On Mac OS X it is easiest to install most of the required software
with MacPorts http://www.macports.org/:

    $ sudo port install \
        boost \
        jpeg \
        png \
        freetype \
        libpng \
        libogg \
        libvorbis \
        pkgconfig \
        scons

SDL and SDL_image should be installed from the official .dmg images:

 * http://www.libsdl.org/download-1.2.php
 * http://www.libsdl.org/projects/SDL_mixer/

Xcftools have to be compiled manually:

 * http://henning.makholm.net/software

The GCC compiler can be obtained by installing Xcode:

 * http://developer.apple.com/technologies/tools/xcode.html


Compilation
-----------

For compilation type:

    $ ./waf configure
    $ ./waf


Running
-------

To run it from inside its source directory use:

 % build/windstille -d data/


Installation
------------

Installation isn't currently supported, run the executables directly from the source tree.

