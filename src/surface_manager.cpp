// Windstille Display Library
// Copyright (C) 2002-2020 Ingo Ruhnke <grumbel@gmail.com>
//
// This program is free software: you can redistribute it and/or modify it
// under the terms of the GNU Lesser General Public License as published by
// the Free Software Foundation, either version 3 of the License, or (at your
// option) any later version.
//
// This program is distributed in the hope that it will be useful, but
// WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY
// or FITNESS FOR A PARTICULAR PURPOSE. See the GNU Lesser General Public
// License for more details.
//
// You should have received a copy of the GNU Lesser General Public License
// along with this program. If not, see <http://www.gnu.org/licenses/>.

#include "surface_manager.hpp"

#include <sstream>
#include <stdexcept>

#include <glm/gtc/round.hpp>

#include "software_surface.hpp"
#include "texture_packer.hpp"

namespace wstdisplay {

SurfaceManager::SurfaceManager() :
  m_texture_packer(),
  m_surfaces()
{
  // NPOV should be ok with OpenGL2.0 in theory, but in practice there
  // is hardware that does OpenGL2.0, but not NPOV, see:
  // http://www.opengl.org/wiki/NPOT_Texture
  if (!GLEW_ARB_texture_non_power_of_two) {
    m_texture_packer.reset(new TexturePacker(geom::isize(2048, 2048)));
  }
}

SurfaceManager::~SurfaceManager()
{
#if 0
  if (debug)
  {
    for(Surfaces::iterator i = surfaces.begin(); i != surfaces.end(); ++i)
    {
      //      if (i->second.use_count() > 1)
      std::cerr << "Warning: Surface '" << i->first << "' not released.\n";
    }
  }
#endif
}

SurfacePtr
SurfaceManager::get(std::filesystem::path const& filename)
{
  if (auto it = m_surfaces.find(filename); it != m_surfaces.end()) {
    // Surface in cache, return it
    return it->second;
  }

  // load Surface from file
  SoftwareSurface software_surface = SoftwareSurface::from_file(filename);

  if (m_texture_packer) {
    SurfacePtr result = m_texture_packer->upload(software_surface);
    m_surfaces[filename] = result;
    return result;
  } else {
    float maxu = 0.0f;
    float maxv = 0.0f;
    TexturePtr texture;

    try {
      texture = create_texture(software_surface, &maxu, &maxv);
    } catch(std::exception& e) {
      std::ostringstream msg;
      msg << "Couldn't create texture for '" << filename << "': " << e.what();
      throw std::runtime_error(msg.str());
    }

    SurfacePtr result = Surface::create(texture, geom::frect(0.0f, 0.0f, maxu, maxv),
                                        geom::fsize(static_cast<float>(software_surface.get_width()),
                                                    static_cast<float>(software_surface.get_height())));
    m_surfaces[filename] = result;
    return result;
  }
}

void
SurfaceManager::load_grid(std::filesystem::path const& filename,
                          std::vector<SurfacePtr>& out_surfaces,
                          geom::isize const& size)
{
  SoftwareSurface const image = SoftwareSurface::from_file(filename);

  float maxu;
  float maxv;
  TexturePtr texture;

  try {
    texture = create_texture(image, &maxu, &maxv);
  } catch(const std::exception& e) {
    std::ostringstream msg;
    msg << "Couldn't create texture for '" << filename << "': " << e.what();
    throw std::runtime_error(msg.str());
  }

  for(int y = 0; y <= image.get_height() - size.height() + 1; y += size.height()) {
    for(int x = 0; x <= image.get_width() - size.width() + 1; x += size.width()) {
      float const s_min_u = maxu * static_cast<float>(x) / static_cast<float>(image.get_width());
      float const s_min_v = maxv * static_cast<float>(x) / static_cast<float>(image.get_height());
      float const s_max_u = (maxu * (static_cast<float>(x + size.width())))  / static_cast<float>(image.get_width());
      float const s_max_v = (maxv * (static_cast<float>(x + size.height()))) / static_cast<float>(image.get_height());

      out_surfaces.push_back(Surface::create(texture,
                                             geom::frect(s_min_u, s_min_v, s_max_u, s_max_v),
                                             geom::fsize(static_cast<float>(size.width()),
                                                   static_cast<float>(size.height()))));
    }
  }
}

TexturePtr
SurfaceManager::create_texture(SoftwareSurface const& image,
                               float* maxu, float* maxv)
{
  // OpenGL2.0 should be fine with non-power-of-two, but some
  // implementations aren't
  if (GLEW_ARB_texture_non_power_of_two) {
    *maxu = 1.0f;
    *maxv = 1.0f;
    return Texture::create(image);
  } else {
    geom::isize texture_size(glm::ceilPowerOfTwo(image.get_width()),
                             glm::ceilPowerOfTwo(image.get_height()));

    SoftwareSurface convert = SoftwareSurface::create(surf::PixelFormat::RGBA8, texture_size);

    surf::blit(image, convert, {0, 0});

    TexturePtr texture = Texture::create(convert);

    *maxu = static_cast<float>(image.get_width())  / static_cast<float>(texture_size.width());
    *maxv = static_cast<float>(image.get_height()) / static_cast<float>(texture_size.height());

    return texture;
  }
}

void
SurfaceManager::cleanup()
{
  std::erase_if(m_surfaces, [](auto const& item) {
    return item.second.use_count() == 0;
  });
}

void
SurfaceManager::save_all_as_png() const
{
  m_texture_packer->save_all_as_png();
}

} // namespace wstdisplay

/* EOF */
