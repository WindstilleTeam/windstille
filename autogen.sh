#!/bin/sh

aclocal-1.7
automake-1.7 --copy --add-missing
autoheader
autoconf

# EOF #