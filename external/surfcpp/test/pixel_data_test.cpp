#include <iostream>
#include <gtest/gtest.h>

#include <geom/rect.hpp>

#include <surf/blend.hpp>
#include <surf/blit.hpp>
#include <surf/color.hpp>
#include <surf/pixel_data.hpp>
#include <surf/sdl.hpp>
#include <surf/transform.hpp>
#include <surf/io.hpp>

#include "plugins/png.hpp"

using namespace surf;

TEST(PixelDataTest, default_is_valid)
{
  PixelData<RGBPixel> const pixel_data;

  EXPECT_EQ(geom::isize(0, 0), pixel_data.get_size());
  EXPECT_EQ(geom::isize(0, 0), halve(pixel_data).get_size());
  EXPECT_EQ(geom::isize(32, 16), scale(pixel_data, geom::isize(32, 16)).get_size());
  EXPECT_EQ(geom::isize(0, 0), crop(pixel_data, geom::irect(0, 0, 16, 16)).get_size());

  transform(pixel_data, Transform::ROTATE_0);
  rotate90(pixel_data);
  rotate180(pixel_data);
  rotate270(pixel_data);
  flip_horizontal(pixel_data);
  flip_vertical(pixel_data);
#if 0
  to_rgb(pixel_data);
  average_color(pixel_data);
#endif
}

TEST(PixelDataTest, creation)
{
  PixelData<RGBPixel> pixeldata;
  EXPECT_TRUE(pixeldata.empty());

  pixeldata = PixelData<RGBPixel>(geom::isize(64, 32));
  EXPECT_FALSE(pixeldata.empty());
  EXPECT_EQ(pixeldata.get_size(), geom::isize(64, 32));
}

TEST(PixelDataTest, equality)
{
  PixelData<RGBPixel> const black(geom::isize(64, 32), RGBPixel{0, 0, 0});
  PixelData<RGBPixel> const white(geom::isize(64, 32), RGBPixel{255, 128, 64});

  PixelData<RGBPixel> const pixeldata(geom::isize(64, 32), RGBPixel{255, 128, 64});
  EXPECT_FALSE(pixeldata.empty());

  EXPECT_EQ(pixeldata.get_size(), geom::isize(64, 32));
  EXPECT_EQ(pixeldata, pixeldata);
  EXPECT_EQ(pixeldata, white);
  EXPECT_NE(pixeldata, black);

  PixelData<RGBAPixel> pixeldata_rgba(geom::isize(64, 32), RGBAPixel{255, 128, 64, 255});
  EXPECT_NE(pixeldata, pixeldata_rgba);
}

TEST(PixelDataTest, blit)
{
  PixelData<RGBPixel> const white(geom::isize(4, 3), RGBPixel{255, 255, 255});

  PixelData<RGBPixel> pixeldata(geom::isize(8, 6), RGBPixel{255, 0, 0});
  PixelData<RGBPixel> pixeldata_expected(geom::isize(8, 6), RGBPixel{255, 255, 255});

  blit(white, pixeldata, geom::ipoint(0, 0));
  blit(white, pixeldata, geom::ipoint(4, 0));
  blit(white, pixeldata, geom::ipoint(0, 3));
  blit(white, pixeldata, geom::ipoint(4, 3));

  EXPECT_EQ(pixeldata, pixeldata_expected);
}

TEST(PixelDataTest, blit_to__srcrect)
{
  PixelData<RGBPixel> const white(geom::isize(8, 6), RGBPixel{255, 255, 255});

  PixelData<RGBPixel> pixeldata(geom::isize(8, 6), RGBPixel{255, 0, 0});
  PixelData<RGBPixel> pixeldata_expected(geom::isize(8, 6), RGBPixel{255, 255, 255});

  blit(white, geom::irect(geom::ipoint(1, 1), geom::isize(4, 3)), pixeldata, geom::ipoint(0, 0));
  blit(white, geom::irect(geom::ipoint(2, 1), geom::isize(4, 3)), pixeldata, geom::ipoint(4, 0));
  blit(white, geom::irect(geom::ipoint(3, 3), geom::isize(4, 3)), pixeldata, geom::ipoint(0, 3));
  blit(white, geom::irect(geom::ipoint(4, 3), geom::isize(4, 3)), pixeldata, geom::ipoint(4, 3));

  EXPECT_EQ(pixeldata, pixeldata_expected);
}

TEST(PixelDataTest, blit_to__convert)
{
  PixelData<RGBAPixel> const white(geom::isize(4, 3), RGBAPixel{255, 255, 255, 255});

  PixelData<RGBPixel> pixeldata(geom::isize(8, 6), RGBPixel{255, 0, 0});
  PixelData<RGBPixel> const pixeldata_expected(geom::isize(8, 6), RGBPixel{255, 255, 255});

  blit(white, pixeldata, geom::ipoint(0, 0));
  blit(white, pixeldata, geom::ipoint(4, 0));
  blit(white, pixeldata, geom::ipoint(0, 3));
  blit(white, pixeldata, geom::ipoint(4, 3));

  EXPECT_EQ(pixeldata, pixeldata_expected);
}

TEST(PixelDataTest, blit_to__srcrect_convert)
{
  PixelData<RGBAPixel> const white(geom::isize(8, 6), RGBAPixel{255, 255, 255, 255});

  PixelData<RGBPixel> pixeldata(geom::isize(8, 6), RGBPixel{255, 0, 0});
  PixelData<RGBPixel> pixeldata_expected(geom::isize(8, 6), RGBPixel{255, 255, 255});

  blit(white, geom::irect(geom::ipoint(1, 1), geom::isize(4, 3)), pixeldata, geom::ipoint(0, 0));
  blit(white, geom::irect(geom::ipoint(2, 1), geom::isize(4, 3)), pixeldata, geom::ipoint(4, 0));
  blit(white, geom::irect(geom::ipoint(3, 3), geom::isize(4, 3)), pixeldata, geom::ipoint(0, 3));
  blit(white, geom::irect(geom::ipoint(4, 3), geom::isize(4, 3)), pixeldata, geom::ipoint(4, 3));

  EXPECT_EQ(pixeldata, pixeldata_expected);
}

TEST(PixelDataTest, convert)
{
  PixelData<RGBPixel> const pixeldata_rgb(geom::isize(64, 32));
  PixelData<RGBAPixel> const pixeldata_rgba = pixeldata_rgb.convert_to<RGBAPixel>();

  EXPECT_EQ(pixeldata_rgb.get_size(), pixeldata_rgba.get_size());
}

TEST(PixelDataTest, blend)
{
  PixelData<RGBAPixel> dst(geom::isize(512, 512), RGBAPixel{255, 255, 255, 255});
  //PixelData<RGBPixel> dst(geom::isize(512, 512), RGBPixel{255, 255, 255});

  SoftwareSurface surface = SoftwareSurface::from_file("test/data/rgba.png");
  PixelView<RGBAPixel> const& src = surface.as_pixelview<RGBAPixel>();

  for (int y = 0; y < dst.get_height(); y += src.get_height()) {
    for (int x = 0; x < dst.get_width(); x += src.get_width()) {
      blend(pixel_blend<RGBAPixel, RGBAPixel>(), src, dst, geom::ipoint(x, y));
    }
  }

  png::save(SoftwareSurface(std::move(dst)), "/tmp/foo2.png");
}

TEST(PixelDataTest, empty)
{
  PixelData<RGBPixel> pixeldata;
  EXPECT_TRUE(pixeldata.empty());

  pixeldata = PixelData<RGBPixel>(geom::isize(64, 32));
  EXPECT_FALSE(pixeldata.empty());

  pixeldata = PixelData<RGBPixel>(geom::isize(0, 0));
  EXPECT_TRUE(pixeldata.empty());
}

TEST(PixelDataTest, create_view__const)
{
  PixelData<RGBPixel> const pixeldata(geom::isize(64, 32));
  std::unique_ptr<IPixelData const> pixelview = pixeldata.create_view(geom::irect(pixeldata.get_size()));
  // This shall not compile:
  // fill(*pixelview, RGBPixel{0, 0, 0});
}

/* EOF */
