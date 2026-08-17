#include <gtest/gtest.h>

#include <geom/size.hpp>
#include <geom/io.hpp>

#include <surf/software_surface.hpp>
#include "plugins/png.hpp"

using namespace surf;

TEST(PNGTest, get_size__mem)
{
}

TEST(PNGTest, get_size__file)
{
  geom::isize result;
  EXPECT_TRUE(png::get_size("test/data/rgb.png", result));
  EXPECT_EQ(result, geom::isize(128, 64));
}

TEST(PNGTest, is_png)
{
  EXPECT_TRUE(png::is_png("test/data/rgb.png"));
  EXPECT_TRUE(png::is_png("test/data/rgba.png"));
  EXPECT_FALSE(png::is_png("test/data/rgb.jpg"));
}

TEST(PNGTest, load_from_file)
{
  png::load_from_file("test/data/rgb.png");
  png::load_from_file("test/data/rgba.png");
}

TEST(PNGTest, load_from_mem)
{
}

TEST(PNGTest, save__file)
{
  SoftwareSurface input_surface = png::load_from_file("test/data/rgb-8x4.png");
  std::filesystem::path outfile = std::filesystem::path(testing::TempDir()) / "PNGTest__save__file.png";
  png::save(input_surface, outfile);

  EXPECT_EQ(input_surface, png::load_from_file(outfile));
}

TEST(PNGTest, save__mem)
{
}

/* EOF */
