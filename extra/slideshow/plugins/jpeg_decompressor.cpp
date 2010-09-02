/*
**  Galapix - an image viewer for large image collections
**  Copyright (C) 2008 Ingo Ruhnke <grumbel@gmx.de>
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

#include "jpeg_decompressor.hpp"

#include <sstream>
#include <iostream>
#include <stdexcept>

void
JPEGDecompressor::fatal_error_handler(j_common_ptr cinfo)
{
  longjmp(reinterpret_cast<ErrorMgr*>(cinfo->err)->setjmp_buffer, 1);
}

JPEGDecompressor::JPEGDecompressor() :
  m_cinfo(),
  m_err()
{
  jpeg_std_error(&m_err.pub);

  m_err.pub.error_exit = &JPEGDecompressor::fatal_error_handler;

  m_cinfo.err = &m_err.pub;

  jpeg_create_decompress(&m_cinfo);
}

JPEGDecompressor::~JPEGDecompressor()
{
  jpeg_destroy_decompress(&m_cinfo);
}

Size
JPEGDecompressor::read_size()
{
  if (setjmp(m_err.setjmp_buffer))
  {
    char buffer[JMSG_LENGTH_MAX];
    (m_cinfo.err->format_message)(reinterpret_cast<jpeg_common_struct*>(&m_cinfo), buffer);

    std::ostringstream out;
    out << "JPEG::read_size(): " /*<< filename << ": "*/ << buffer;
    throw std::runtime_error(out.str());
  }
  else
  {
    jpeg_read_header(&m_cinfo, /*require_image*/ FALSE);

    return Size(m_cinfo.image_width,
                m_cinfo.image_height);
  }
}

/* EOF */
