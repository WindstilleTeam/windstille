#!/bin/sh

GLADE_CATALOG_PATH=catalog/
export GLADE_CATALOG_PATH

GLADE_MODULE_PATH=module/
export GLADE_MODULE_PATH

exec ./editor

# EOF #
