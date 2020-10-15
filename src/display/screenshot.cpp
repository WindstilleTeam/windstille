/*
**  Windstille - A Sci-Fi Action-Adventure Game
**  Copyright (C) 2018 Ingo Ruhnke <grumbel@gmail.com>
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

#include "display/screenshot.hpp"

#include <iostream>
#include <errno.h>
#include <fstream>
#include <jpeglib.h>
#include <png.h>
#include <sstream>
#include <stdexcept>
#include <string.h>

#include <geom/rect.hpp>
#include <geom/line.hpp>
#include <geom/quad.hpp>

#include "display/color.hpp"
#include "display/opengl_state.hpp"
#include "display/assert_gl.hpp"

void save_screenshot(std::filesystem::path const& filename)
{
  GLint viewport[4];
  glGetIntegerv(GL_VIEWPORT, viewport);
  geom::isize size(viewport[2] / 4 * 4, viewport[3]);

  int len = size.width() * size.height() * 3;

  std::vector<GLbyte> pixels(len);
  glPixelStorei(GL_PACK_ALIGNMENT, 1);
  glReadPixels(0, 0, size.width(), size.height(), GL_RGB, GL_UNSIGNED_BYTE, pixels.data());
  assert_gl("Display::save_screenshot()");

  if ((false))
  { // PPM saving
    int pitch = size.width() * 3;

    // save to ppm
    std::ofstream out(filename);
    out << "P6\n"
        << "# Windstille Screenshot\n"
        << size.width() << " " << size.height() << "\n"
        << "255\n";

    for(int y = size.height() - 1; y >= 0; --y)
      out.write(reinterpret_cast<const char*>(pixels.data() + y*pitch), pitch);

    out.close();
  }
  else if ((true))
  {
    FILE* m_out = fopen(filename.c_str(), "wb");

    if (!m_out)
    {
      std::ostringstream out;
      out << "FileJPEGCompressor(): Error: " << filename << ": " << strerror(errno);
      throw std::runtime_error(out.str());
    }
    else
    {
      int pitch = size.width() * 3;
      struct jpeg_compress_struct m_cinfo;
      struct jpeg_error_mgr m_jerr;

      jpeg_std_error(&m_jerr);
      m_cinfo.err = &m_jerr;

      jpeg_create_compress(&m_cinfo);

      jpeg_stdio_dest(&m_cinfo, m_out);

      m_cinfo.image_width  = size.width();
      m_cinfo.image_height = size.height();

      m_cinfo.input_components = 3;         /* # of color components per pixel */
      m_cinfo.in_color_space   = JCS_RGB;   /* colorspace of input image */

      jpeg_set_defaults(&m_cinfo);
      //jpeg_set_quality(&m_cinfo, 100, TRUE /* limit to baseline-JPEG values */);

      jpeg_start_compress(&m_cinfo, TRUE);

      std::vector<JSAMPROW> row_pointer(size.height());

      for(int y = 0; y < size.height(); ++y)
      {
        row_pointer[size.height() - y - 1] = reinterpret_cast<JSAMPLE*>(pixels.data() + y*pitch);
      }

      while(m_cinfo.next_scanline < m_cinfo.image_height)
      {
        jpeg_write_scanlines(&m_cinfo, &row_pointer[m_cinfo.next_scanline],
                             size.height() - m_cinfo.next_scanline);
      }

      jpeg_finish_compress(&m_cinfo);

      jpeg_destroy_compress(&m_cinfo);

      fclose(m_out);
    }
  }
  else if ((false))
  { // PNG saving
    FILE* fp = fopen(filename.c_str(), "w");

    if (!fp)
    {
      std::cout << "Error: Couldn't save screenshot: " << strerror(errno) << std::endl;
      return;
    }
    else
    {
      int pitch = size.width() * 3;
      png_structp png_ptr;
      png_infop   info_ptr;

      png_ptr  = png_create_write_struct(PNG_LIBPNG_VER_STRING, nullptr, nullptr, nullptr);
      info_ptr = png_create_info_struct(png_ptr);

      png_init_io(png_ptr, fp);

      png_set_IHDR(png_ptr, info_ptr,
                   size.width(), size.height(), 8 /* bitdepth */,
                   PNG_COLOR_TYPE_RGB,
                   PNG_INTERLACE_NONE,
                   PNG_COMPRESSION_TYPE_BASE,
                   PNG_FILTER_TYPE_BASE);

      png_set_compression_level(png_ptr, 0);
      png_write_info(png_ptr, info_ptr);

      std::vector<png_bytep> row_pointers(size.height());

      // generate row pointers
      for (int k = 0; k < size.height(); k++)
        row_pointers[k] = reinterpret_cast<png_byte*>(pixels.data() + ((size.height() - k - 1) * pitch));

      png_write_image(png_ptr, row_pointers.data());

      png_write_end(png_ptr, info_ptr);

      fclose(fp);
    }
  }
}

/* EOF */
