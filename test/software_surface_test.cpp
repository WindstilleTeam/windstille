#include <iostream>

#include <surf/save.hpp>
#include "display/software_surface.hpp"

int main(int argc, char** argv)
{
  for(int i = 1; i < argc; ++i)
  {
    std::filesystem::path filename = argv[i];
    SoftwareSurface surface = SoftwareSurface::from_file(filename);
    surf::save(surface, "/tmp/out.png", "png");
  }

  return 0;
}

/* EOF */
