#include <iostream>

#include "display/software_surface.hpp"

int main(int argc, char** argv)
{
  for(int i = 1; i < argc; ++i)
  {
    std::filesystem::path filename = argv[i];
    SoftwareSurfacePtr surface = SoftwareSurface::create(filename);
    surface->save_png("/tmp/out.png");
  }

  return 0;
}

/* EOF */
