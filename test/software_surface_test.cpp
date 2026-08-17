#include <iostream>
#include <gtest/gtest.h>

#include <geom/rect.hpp>
#include <geom/io.hpp>

#include <surf/palette.hpp>
#include <surf/software_surface.hpp>
#include <surf/transform.hpp>

using namespace surf;

TEST(SoftwareSurfaceTest, default_is_valid)
{
  SoftwareSurface const pixel_data;

  EXPECT_EQ(geom::isize(0, 0), pixel_data.get_size());

#if 0
  EXPECT_EQ(geom::isize(0, 0), halve(pixel_data).get_size());
  EXPECT_EQ(geom::isize(0, 0), scale(pixel_data, geom::isize(32, 16)).get_size());
  EXPECT_EQ(geom::isize(0, 0), crop(pixel_data, geom::irect(0, 0, 16, 16)).get_size());

  transform(pixel_data, Transform::ROTATE_0);
  rotate90(pixel_data);
  rotate180(pixel_data);
  rotate270(pixel_data);
  flip_horizontal(pixel_data);
  flip_vertical(pixel_data);
  SUCCEED();

  to_rgb(pixel_data);
  average_color(pixel_data);
#endif
}

TEST(SoftwareSurfaceTest, assignment)
{
  SoftwareSurface lhs(PixelData<RGBPixel>(geom::isize(32, 16)));
  SoftwareSurface rhs(PixelData<RGBPixel>(geom::isize(64, 32)));
  SoftwareSurface tmp;

  tmp = lhs;
  lhs = rhs;
  rhs = tmp;

  EXPECT_EQ(geom::isize(64, 32), lhs.get_size());
  EXPECT_EQ(geom::isize(32, 16), rhs.get_size());
}

TEST(SoftwareSurfaceTest, move)
{
  SoftwareSurface lhs(PixelData<RGBPixel>(geom::isize(32, 16)));
  SoftwareSurface rhs(PixelData<RGBPixel>(geom::isize(64, 32)));
  SoftwareSurface tmp;

  tmp = std::move(lhs);
  lhs = std::move(rhs);
  rhs = std::move(tmp);

  EXPECT_EQ(geom::isize(64, 32), lhs.get_size());
  EXPECT_EQ(geom::isize(32, 16), rhs.get_size());

  // this will crash as SoftwareSurface::m_pixel_data goes nullptr
  // EXPECT_EQ(geom::isize(32, 16), tmp.get_size());
}

TEST(SoftwareSurfaceTest, convert)
{
  SoftwareSurface const lhs(PixelData<RGBPixel>(geom::isize(32, 16)));
  SoftwareSurface rhs = convert(lhs, PixelFormat::RGBA8);

  EXPECT_EQ(lhs.get_size(), rhs.get_size());
}

TEST(SoftwareSurfaceTest, blit)
{
  SoftwareSurface const src(PixelData<RGB8Pixel>(geom::isize(4, 2), {255, 0, 0}));
  SoftwareSurface dst(PixelData<RGBA8Pixel>(geom::isize(8, 4), {0, 0, 0, 0}));

  blit(src, dst, geom::ipoint(1, 2));
}

TEST(SoftwareSurfaceTest, fill)
{
  SoftwareSurface const src(PixelData<RGB8Pixel>(geom::isize(4, 2), {255, 0, 0}));
  SoftwareSurface dst(PixelData<RGBA8Pixel>(geom::isize(8, 4), {0, 0, 0, 0}));

  fill(dst, palette::white);
}

TEST(SoftwareSurfaceTest, fill_rect)
{
  SoftwareSurface const src(PixelData<RGB8Pixel>(geom::isize(4, 2), {255, 0, 0}));
  SoftwareSurface dst(PixelData<RGBA8Pixel>(geom::isize(8, 4), {0, 0, 0, 0}));

  fill_rect(dst, geom::irect(1, 2, 4, 4), palette::white);
}

TEST(SoftwareSurfaceTest, get_view)
{
  SoftwareSurface const src(PixelData<RGB8Pixel>(geom::isize(8, 6), {255, 0, 0}));
  SoftwareSurface view(src.get_view(geom::irect(2, 2, 5, 5)));
  fill(view, Color(0, 0, 0, 0));
}

TEST(SoftwareSurfaceTest, create_view)
{
  PixelData<RGB8Pixel> pixeldata(geom::isize(8, 4), {255, 0, 0});
  SoftwareSurface view = SoftwareSurface::create_view(pixeldata);
  fill(view, Color(0, 0, 0, 0));
}

TEST(SoftwareSurfaceTest, create_view__const)
{
  PixelData<RGB8Pixel> const pixeldata(geom::isize(8, 4), {255, 0, 0});
  SoftwareSurface view = SoftwareSurface::create_view(pixeldata);
  fill(view, Color(0, 0, 0, 0));
}

/* EOF */
