[![Build Status](https://travis-ci.org/WindstilleTeam/windstille.svg?branch=master)](https://travis-ci.org/WindstilleTeam/windstille)

Windstille
==========

Windstille is a cinematic platformer in which the player will be
placed in a foreign alien world and has to find its way back into
safety. Windstille is currently available for GNU/Linux and Windows.
Its webpage at which one might find new versions, source code,
artworks and more informations is located at:

* https://github.com/WindstilleTeam/

The latest development version can be obtained by:

* https://github.com/WindstilleTeam/windstille

For informations on how to compile and run Windstille see the file
INSTALL. Windstille makes use of OpenGL so you will need working
hardware 3d support.

Windstille is covered under the GNU GPL 3+, which means that you can copy
and even modify it pretty much as you like, as long as you keep the
copyright headers in place and distribute the source too if you
distribute binaries, see the file COPYING for details.


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
 * cmake
 * bison (bison++ will not work)
 * flex
 * gtkmm (for the editor only)
 * gtkglextmm (for the editor only)
 * glm

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
        cmake \
        libpng-dev \
        libopenal-dev \
        libgtkmm-2.4-dev \
        libgtkglextmm-x11-1.2-dev \
        libmagick++-dev \
        xcftools \
        libglew1.5-dev \
        libogg-dev \
        libvorbis-dev \
        libglm-dev

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
        cmake

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

    mkdir build
    cd build
    cmake ..
    make

Building the editor can be enabled via the `cmake` flag
`-DBUILD_EDITOR=1`. Tests and extras can be build with
`-DBUILD_TESTS=1` and `-DBUILD_EXTRA=1`. Extra verbose warnings are
enabled with `-DWARNINGS=1`.


Running
-------

To run it from inside its source directory use:

 % build/windstille -d build/data/


Installation
------------

    make install

Configuration of install location is available via the `cmake` flag
`-DCMAKE_INSTALL_PREFIX:PATH=...` and by passing `DESTDIR=...` to
`make`.
