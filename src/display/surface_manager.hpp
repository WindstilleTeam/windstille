#ifndef __SURFACE_MANAGER_H__
#define __SURFACE_MANAGER_H__

#include <string>
#include <vector>
#include <map>
#include <SDL.h>
#include <GL/glew.h>
#include <GL/gl.h>
#include "ref.hpp"
#include "texture.hpp"

class Surface;

/**
 * This class keeps a list of loaded surfaces and manages loading new ones
 */
class SurfaceManager
{
public:
  SurfaceManager();
  ~SurfaceManager();

  /** returns a surface containing the image specified with filename */
  Surface get(const std::string& filename);

  /**
   * Loads an image and splits it into several Surfaces sized width and height.
   * The created surfaces will be added to the surfaces vector.
   */
  void load_grid(const std::string& filename,
      std::vector<Surface>& surfaces, int width, int height);

  Texture create_texture(SDL_Surface* image, 
                         float& maxu, float& maxv);

  /** Removes all cached Sprites that are no longer in use */
  void cleanup();
  
  typedef std::map<std::string, Surface> Surfaces;
  Surfaces surfaces;
};

extern SurfaceManager* surface_manager;

#endif

