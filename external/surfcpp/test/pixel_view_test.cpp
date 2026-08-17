#include <iostream>
#include <gtest/gtest.h>

#include <geom/rect.hpp>

#include <surf/blend.hpp>
#include <surf/blit.hpp>
#include <surf/fill.hpp>
#include <surf/color.hpp>
#include <surf/pixel_data.hpp>
#include <surf/sdl.hpp>
#include <surf/transform.hpp>
#include <surf/io.hpp>

#include "plugins/png.hpp"

using namespace surf;

TEST(PixelViewTest, get_view)
{
  PixelData<RGBPixel> const black(geom::isize(16, 8), RGBPixel{0, 0, 0});

  PixelData<RGBPixel> expected(geom::isize(16, 8), RGBPixel{0xde, 0xad, 0xff});
  fill_rect(expected, geom::irect(1, 1, 15, 7), RGBPixel{0, 0, 0});

  PixelData<RGBPixel> pixeldata(geom::isize(16, 8), RGBPixel{0xde, 0xad, 0xff});
  PixelView<RGBPixel> pixelview = pixeldata.get_view(geom::irect(1, 1, 15, 7));
  blit(black, pixelview, geom::ipoint(0, 0));

  EXPECT_EQ(pixeldata, expected);
}

/* EOF */
