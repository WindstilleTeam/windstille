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

#ifndef HEADER_SURF_BLEND_HPP
#define HEADER_SURF_BLEND_HPP

#include <logmich/log.hpp>

#include "color.hpp"
#include "convert.hpp"
#include "pixel.hpp"

namespace surf {

template<typename SrcPixel, typename DstPixel>
struct pixel_copy
{
  inline DstPixel operator()(SrcPixel src, DstPixel dst)
  {
    return convert<SrcPixel, DstPixel>(src);
  }
};

template<typename SrcPixel, typename DstPixel>
struct pixel_blend
{
  inline DstPixel operator()(SrcPixel src, DstPixel dst)
  {
    using srctype = typename SrcPixel::value_type;
    using dsttype = typename DstPixel::value_type;

    if constexpr (!SrcPixel::has_alpha()) {
      return convert<SrcPixel, DstPixel>(src);
    } else if constexpr (std::is_floating_point<srctype>::value || std::is_floating_point<dsttype>::value) {
      log_not_implemented();
      return convert<SrcPixel, DstPixel>(src);
    } else if constexpr (SrcPixel::has_alpha() && !DstPixel::has_alpha()) {
      return make_pixel<DstPixel>(
        static_cast<dsttype>((red(src) * alpha(src) + red(dst) * (SrcPixel::max() - alpha(src))) / SrcPixel::max()),
        static_cast<dsttype>((green(src) * alpha(src) + green(dst) * (SrcPixel::max() - alpha(src))) / SrcPixel::max()),
        static_cast<dsttype>((blue(src) * alpha(src) + blue(dst) * (SrcPixel::max() - alpha(src))) / SrcPixel::max())
        );
    } else if constexpr (SrcPixel::has_alpha() && DstPixel::has_alpha()) {
      dsttype const out_a = static_cast<dsttype>(alpha(src) + alpha(dst) * (SrcPixel::max() - alpha(src)) / SrcPixel::max());
      if (out_a == 0) {
        return make_pixel<DstPixel>(0, 0, 0, 0);
      } else {
        return make_pixel<DstPixel>(
          static_cast<dsttype>((red(src) * alpha(src) * DstPixel::max() / SrcPixel::max() + red(dst) * alpha(dst) * (SrcPixel::max() - alpha(src)) / SrcPixel::max()) / out_a),
          static_cast<dsttype>((green(src) * alpha(src) * DstPixel::max() / SrcPixel::max() + green(dst) * alpha(dst) * (SrcPixel::max() - alpha(src)) / SrcPixel::max()) / out_a),
          static_cast<dsttype>((blue(src) * alpha(src) * DstPixel::max() / SrcPixel::max() + blue(dst) * alpha(dst) * (SrcPixel::max() - alpha(src)) / SrcPixel::max()) / out_a),
          out_a
          );
      }
    } else {
      static_assert(!std::is_same<SrcPixel, SrcPixel>::value,
                    "blend<>() not implemented for the given types");
      return DstPixel{};
    }
  }
};

template<typename SrcPixel, typename DstPixel>
struct pixel_add
{
  inline constexpr DstPixel operator()(SrcPixel const src, DstPixel const dst)
  {
    // using srctype = typename SrcPixel::value_type;
    using dsttype = typename DstPixel::value_type;

    if constexpr (SrcPixel::has_alpha()) {
      if (alpha(src) == 0) {
        return dst;
      }
    }

    if constexpr (std::is_floating_point<dsttype>::value) {
      return make_pixel<DstPixel>(
        static_cast<dsttype>(red_f(dst) + red_f(src) * alpha_f(src)),
        static_cast<dsttype>(green_f(dst) + green_f(src) * alpha_f(src)),
        static_cast<dsttype>(blue_f(dst) + blue_f(src) * alpha_f(src)),
        static_cast<dsttype>(alpha_f(dst))
        );
    } else {
      dsttype const r = convert_value<SrcPixel, DstPixel>(red(src));
      dsttype const g = convert_value<SrcPixel, DstPixel>(green(src));
      dsttype const b = convert_value<SrcPixel, DstPixel>(blue(src));
      dsttype const a = convert_value<SrcPixel, DstPixel>(alpha(src));

      return make_pixel<DstPixel>(
        clamp_pixel_max<DstPixel>(red(dst) + r * a / DstPixel::max()),
        clamp_pixel_max<DstPixel>(green(dst) + g * a / DstPixel::max()),
        clamp_pixel_max<DstPixel>(blue(dst) + b * a / DstPixel::max()),
        alpha(dst));
    }
  }
};

template<typename SrcPixel, typename DstPixel>
struct pixel_multiply
{
  inline constexpr DstPixel operator()(SrcPixel const src, DstPixel const dst)
  {
    // using srctype = typename SrcPixel::value_type;
    using dsttype = typename DstPixel::value_type;

    if constexpr (SrcPixel::has_alpha()) {
      if (alpha(src) == 0) {
        return dst;
      }
    }

    // FIXME: ignoring src alpha for now
    if constexpr (std::is_floating_point<dsttype>::value) {
      return make_pixel<DstPixel>(
        static_cast<dsttype>(red_f(dst) * red_f(src)),
        static_cast<dsttype>(green_f(dst) * green_f(src)),
        static_cast<dsttype>(blue_f(dst) * blue_f(src)),
        static_cast<dsttype>(alpha_f(dst))
        );
    } else {
      dsttype const r = convert_value<SrcPixel, DstPixel>(red(src));
      dsttype const g = convert_value<SrcPixel, DstPixel>(green(src));
      dsttype const b = convert_value<SrcPixel, DstPixel>(blue(src));
      //dsttype const a = convert_value<SrcPixel, DstPixel>(alpha(src));

      return make_pixel<DstPixel>(
        clamp_pixel_max<DstPixel>(red(dst) * r / DstPixel::max()),
        clamp_pixel_max<DstPixel>(green(dst) * g / DstPixel::max()),
        clamp_pixel_max<DstPixel>(blue(dst) * b / DstPixel::max()),
        alpha(dst));
    }
  }
};

} // namespace surf

#endif

/* EOF */
