#ifndef __TEXTURE_MANAGER_H__
#define __TEXTURE_MANAGER_H__

#include <string>
#include <map>
#include <SDL.h>
#include <GL/glew.h>
#include <GL/gl.h>

class Texture;

/**
 * This class manages a map of image files to OpenGL textures.
 */
class TextureManager
{
public:
  TextureManager();
  ~TextureManager();

  /**
   * returns a texture containing the image specified with filename
   * The TextureManager keeps track of loaded Textures and might return a
   * reference to an existing texture.
   * Only textures with power of 2 dimensions are supported here. Use
   * SurfaceManager for images with other dimensions.
   * Note: Texture is a refcounted class, store it with Ref<Texture>
   */
  Texture get(const std::string& filename);

private:
  typedef std::map<std::string, Texture> Textures;
  Textures textures;
};

extern TextureManager* texture_manager;

#endif

