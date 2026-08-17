// surf - Software surface library
// Copyright (C) 2008-2020 Ingo Ruhnke <grumbel@gmail.com>
//
// This program is free software: you can redistribute it and/or modify it
// under the terms of the GNU Lesser General Public License as published by
// the Free Software Foundation, either version 3 of the License, or (at your
// option) any later version.
//
// This program is distributed in the hope that it will be useful, but
// WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY
// or FITNESS FOR A PARTICULAR PURPOSE. See the GNU Lesser General Public
// License for more details.
//
// You should have received a copy of the GNU Lesser General Public License
// along with this program. If not, see <http://www.gnu.org/licenses/>.

#ifndef HEADER_SURF_PIXEL_VIEW_HPP
#define HEADER_SURF_PIXEL_VIEW_HPP

#include <vector>

#include <geom/point.hpp>
#include <geom/rect.hpp>
#include <geom/size.hpp>

#include "color.hpp"
#include "convert.hpp"
#include "ipixel_data.hpp"
#include "pixel.hpp"
#include "pixel_format.hpp"

namespace surf {

template<typename Pixel>
class PixelData;

/** A mutable low-level container for pixel data */
template<typename Pixel>
class PixelView : public IPixelData
{
public:
  using value_type = Pixel;

public:
  PixelView() :
    m_size(0, 0),
    m_row_length(0),
    m_pixels(nullptr)
  {}

  ~PixelView() override
  {
    // nothing to delete, m_pixel isn't owned
  }

  PixelView(PixelView<Pixel> const& other) = default;
  PixelView& operator=(PixelView<Pixel> const& other) = default;

  PixelView(geom::isize const& size, Pixel* pixels) :
    m_size(size),
    m_row_length(m_size.width()),
    m_pixels(pixels)
  {}

  PixelView(geom::isize const& size, Pixel* pixels, int row_length) :
    m_size(size),
    m_row_length(row_length),
    m_pixels(pixels)
  {}

  PixelView(geom::isize const& size, Pixel const* pixels, int row_length) :
    m_size(size),
    m_row_length(row_length),
    // FIXME: this is ugly and dangerous, but less troublesome than
    // trying to make PixelView<Pixel const> work.
    m_pixels(const_cast<Pixel*>(pixels))
  {}

  PixelFormat get_format() const override { return PPixelFormat<Pixel>::format; }

  geom::isize get_size() const override { return m_size; }
  int get_width() const override { return m_size.width(); }
  int get_height() const override { return m_size.height(); }
  int get_row_length() const override { return m_row_length; }
  int get_pitch() const override { return m_row_length * sizeof(Pixel); }

  bool empty() const override { return m_pixels == nullptr; }

  void put_pixel(geom::ipoint const& pos, Pixel const& pixel)
  {
    assert(geom::contains(m_size, pos));
    m_pixels[pos.y() * m_row_length + pos.x()] = pixel;
  }

  void put_pixel_color(geom::ipoint const& pos, Color const& color) override
  {
    put_pixel(pos, convert<Color, Pixel>(color));
  }

  Color get_pixel_color(geom::ipoint const& pos) const override
  {
    return convert<Pixel, Color>(get_pixel(pos));
  }

  Pixel get_pixel(geom::ipoint const& pos) const
  {
    assert(geom::contains(m_size, pos));
    return m_pixels[pos.y() * m_row_length + pos.x()];
  }

  Pixel* get_data() {
    return m_pixels;
  }

  Pixel* get_row(int y) {
    return m_pixels + (y * m_row_length);
  }

  void* get_row_data(int y) override {
    return get_row(y);
  }

  Pixel const* get_data() const {
    return m_pixels;
  }

  Pixel const* get_row(int y) const {
    return m_pixels + (y * m_row_length);
  }

  void const* get_row_data(int y) const override {
    return get_row(y);
  }

  template<typename DstPixel>
  PixelData<DstPixel> convert_to() const
  {
    if constexpr (std::is_same<DstPixel, Pixel>::value) {
      return *this;
    } else {
      std::vector<DstPixel> dstpixels;
      dstpixels.reserve(geom::area(this->m_size));

      for (int y = 0; y < m_size.height(); ++y) {
        std::transform(get_row(y), get_row(y) + m_size.width(),
                       std::back_inserter(dstpixels),
                       convert<Pixel, DstPixel>);
      }

      PixelData<DstPixel> result(m_size, std::move(dstpixels));
      return result;
    }
  }

  PixelView<Pixel> get_view(geom::irect const& rect) {
    if (!contains(geom::irect(m_size), rect)) {
      throw std::invalid_argument("rect not within the SoftwareSurface area");
    }

    return PixelView<Pixel>(rect.size(),
                            get_row(rect.top()) + rect.left(),
                            m_row_length);
  }

  std::unique_ptr<IPixelData> create_view(geom::irect const& rect) override
  {
    return std::make_unique<PixelView<Pixel>>(rect.size(),
                                              get_row(rect.top()) + rect.left(),
                                              m_row_length);
  }

  std::unique_ptr<IPixelData const> create_view(geom::irect const& rect) const override
  {
    return std::make_unique<PixelView<Pixel> const>(rect.size(),
                                                    get_row(rect.top()) + rect.left(),
                                                    m_row_length);
  }

  std::unique_ptr<IPixelData> copy() const override {
    return std::make_unique<PixelData<Pixel>>(*this);
  }

protected:
  bool is_equal(IPixelData const& rhs) const override {
    PixelView<Pixel> const* rhs_ptr = dynamic_cast<PixelView<Pixel> const*>(&rhs);
    if (rhs_ptr == nullptr) {
      return false;
    } else if (m_size != rhs_ptr->m_size) {
      return false;
    } else {
      for (int y = 0; y < m_size.height(); ++y) {
        if (!std::equal(get_row(y), get_row(y) + m_size.width(),
                        rhs_ptr->get_row(y))) {
          return false;
        }
      }
    }
    return true;
  }

protected:
  geom::isize m_size;
  int m_row_length;
  Pixel* m_pixels;
};

} // namespace surf

#endif

/* EOF */
