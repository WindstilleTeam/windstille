#!/bin/sh

set -e

doxygen Doxyfile
# rsync -vrPLtC api/ grumbel@shell.berlios.de:/home/groups/windstille/htdocs/docs/
rsync -vrPLtC api/ grumbel@pingus.seul.org:/home/grumbel/public_html/windstille/docs/

# EOF #
