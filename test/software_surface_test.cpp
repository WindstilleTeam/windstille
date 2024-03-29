#include <iostream>

#include <surf/save.hpp>
#include <wstdisplay/software_surface.hpp>

int main(int argc, char** argv)
{
  for(int i = 1; i < argc; ++i)
  {
    std::filesystem::path filename = argv[i];
    surf::SoftwareSurface surface = surf::SoftwareSurface::from_file(filename);
    surf::save(surface, "/tmp/out.png", "png");
  }

  return 0;
}

/* EOF */
