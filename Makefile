##  Windstille - A Sci-Fi Action-Adventure Game
##  Copyright (C) 2010 Ingo Ruhnke <grumbel@gmx.de>
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

DESTDIR = 
PREFIX  = "/usr/local"
DATADIR = "${PREFIX}/share/windstille"
BINDIR  = "${PREFIX}/bin"

build/windstille:
	scons

clean:
	scons -c

install: build/windstille
	install -d "${DESTDIR}${BINDIR}"
	echo "#!/bin/sh\nexec \"${BINDIR}/windstille.bin\" --datadir \"${DATADIR}/\"" > "${DESTDIR}${BINDIR}/windstille"
	chmod 755 "${DESTDIR}${BINDIR}/windstille"
	install -D build/windstille "${DESTDIR}${BINDIR}/windstille.bin"
	cd data/; \
	find -type f \( \
	-name "*.arm" -o \
	-name "*.frag" -o \
	-name "*.jpg" -o \
	-name "*.mesh" -o \
	-name "*.nav" -o \
	-name "*.nut" -o \
	-name "*.ogg" -o \
	-name "*.particles" -o \
	-name "*.png" -o \
	-name "*.po" -o \
	-name "*.pose" -o \
	-name "*.scm" -o \
	-name "*.sprite" -o \
	-name "*.svg" -o \
	-name "*.ttf" -o \
	-name "*.wav" -o \
	-name "*.wsprite" -o \
	-name "*.wst" -o \
	-name "*.xcf" \) \
	-exec install -D {} ${DESTDIR}${DATADIR}/{} \;

.PHONY : clean install

# EOF #
