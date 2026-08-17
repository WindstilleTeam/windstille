#include <iostream>
#include <sstream>
#include <gtest/gtest.h>

#include <geom/rect.hpp>
#include <geom/io.hpp>

#include <surf/pixel_data.hpp>
#include <surf/sdl.hpp>
#include <surf/transform.hpp>

using namespace surf;

TEST(SDLTest, create_sdl_surface_view)
{
  PixelData<RGBPixel> const red(geom::isize{8, 16}, RGBPixel{255, 0, 0});
  PixelData<RGBPixel> const white(geom::isize{8, 16}, RGBPixel{255, 255, 255});
  PixelData<RGBPixel> const pixeldata = white; // NOLINT

  SDLSurfacePtr sdl_surf = create_sdl_surface_view(pixeldata);

  EXPECT_NE(sdl_surf.get(), nullptr);

  EXPECT_EQ(white, pixeldata);

  SDL_FillRect(sdl_surf.get(), nullptr, SDL_MapRGB(sdl_surf->format, 255, 0, 0));

  EXPECT_EQ(red, pixeldata);
}

TEST(SDLTest, create_sdl_surface)
{
  PixelData<RGBPixel> pixeldata({256, 128});
  SDLSurfacePtr sdl_surf = create_sdl_surface_view(pixeldata);
}

TEST(SDLTest, from_sdl_surface)
{
  PixelData<RGBPixel> const red(geom::isize{8, 16}, RGBPixel{255, 0, 0});
  SDLSurfacePtr sdl_red = create_sdl_surface(red);
  PixelData<RGBPixel> const red_out = pixeldata_from_sdl_surface<RGBPixel>(*sdl_red);
  EXPECT_EQ(red, red);
  EXPECT_EQ(red_out, red_out);
  EXPECT_EQ(red, red_out);
}

/* EOF */
