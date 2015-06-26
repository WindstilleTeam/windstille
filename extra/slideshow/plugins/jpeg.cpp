/*
**  Galapix - an image viewer for large image collections
**  Copyright (C) 2008 Ingo Ruhnke <grumbel@gmail.com>
**
**  This program is free software: you can redistribute it and/or modify
**  it under the terms of the GNU General Public License as published by
**  the Free Software Foundation, either version 3 of the License, or
**  (at your option) any later version.
**
**  This program is distributed in the hope that it will be useful,
**  but WITHOUT ANY WARRANTY; without even the implied warranty of
**  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
**  GNU General Public License for more details.
**
**  You should have received a copy of the GNU General Public License
**  along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/

#include <iostream>
#include <sstream>
#include <boost/bind.hpp>
#include <setjmp.h>

#include "math/size.hpp"
#include "file_jpeg_decompressor.hpp"
#include "jpeg.hpp"
#include "util/util.hpp"

bool
JPEG::filename_is_jpeg(const std::string& filename)
{
  // FIXME: Merge this with util/jpeg_software_surface_loader, maybe
  // store the fileformat/SoftwareSurfaceLoader in the database intead
  // of figuring out the format each time anew
  return (has_suffix(filename, "jpg") ||
          has_suffix(filename, "jpeg"));
}

Size
JPEG::get_size(const std::string& filename)
{
  FileJPEGDecompressor loader(filename);
  Size size = loader.read_size();
  return size;
}

/* EOF */
