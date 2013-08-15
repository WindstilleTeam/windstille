#include <iostream>

#include "display/software_surface.hpp"

int main(int argc, char** argv)
{
  for(int i = 1; i < argc; ++i)
  {
    Pathname filename(argv[i], Pathname::kSysPath);
    SoftwareSurfacePtr surface = SoftwareSurface::create(filename);
    surface->save_png("/tmp/out.png");
  }

  return 0;
}

/* EOF */
