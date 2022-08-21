/*
**  Windstille - A Sci-Fi Action-Adventure Game
**  Copyright (C) 2005,2007 Matthias Braun <matze@braunis.de>,
**                          Ingo Ruhnke <grumbel@gmail.com>
**
**  This program is free software: you can redistribute it and/or modify
**  it under the terms of the GNU General Public License as published by
**  the Free Software Foundation, either version 3 of the License, or
**  (at your option) any later version.
**
**  This program is distributed in the hope that it will be useful,
**  but WITHOUT ANY WARRANTY; without even the implied warranty of
**  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
**  GNU General Public License for more details.
**
**  You should have received a copy of the GNU General Public License
**  along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/

#include "sprite2d/data.hpp"

#include <memory>
#include <stdexcept>

#include <wstdisplay/surface_manager.hpp>
#include "util/file_reader.hpp"

namespace windstille {

SpriteData::SpriteData(std::filesystem::path const& filename,
                       wstdisplay::SurfaceManager& surface_manager) :
  actions()
{
  if (std::filesystem::exists(filename))
  {
    std::string const ext = filename.extension().string();

    if (ext == ".sprite")
    {
      ReaderDocument doc = ReaderDocument::from_file(filename);

      if (doc.get_name() != "sprite") {
        std::ostringstream msg;
        msg << "File " << filename << " is not a windstille sprite";
        throw std::runtime_error(msg.str());
      }

      parse(filename.parent_path(), doc.get_mapping(), surface_manager);
    }
    else if (ext == ".png" || ext == ".jpg")
    {
      std::unique_ptr<SpriteAction> action(new SpriteAction());
      action->name   = "default";
      action->speed  = 1.0;
      action->scale  = 1.0f;
      action->offset = glm::vec2(0, 0);
      action->surfaces.push_back(surface_manager.get(filename));
      actions.push_back(action.release());
    }
    else
    {
      std::ostringstream str;
      str << "Sprite " << filename << " has unknown suffix: '" << ext  << "'";
      throw std::runtime_error(str.str());
    }
  }
  else if (filename.string().length() > std::string(".sprite").length())
  { // If sprite file is not found, we search for a file with the
    // same name ending in .png
    std::filesystem::path pngfile = filename;
    pngfile.replace_extension(".png");

    if (std::filesystem::exists(pngfile))
    {
      std::unique_ptr<SpriteAction> action(new SpriteAction);
      action->name   = "default";
      action->speed  = 1.0;
      action->scale  = 1.0f;
      action->offset = glm::vec2(0, 0);
      action->surfaces.push_back(surface_manager.get(pngfile));
      actions.push_back(action.release());
    }
    else
    {
      std::ostringstream str;
      str << "Couldn't find " << pngfile;
      throw std::runtime_error(str.str());
    }
  }
  else
  {
    std::ostringstream str;
    str << "Couldn't find " << filename;
    throw std::runtime_error(str.str());
  }
}

SpriteData::~SpriteData()
{
  for(Actions::iterator i = actions.begin(); i != actions.end(); ++i)
    delete *i;
}

void
SpriteData::parse(std::filesystem::path const& dir, ReaderMapping const& reader,
                  wstdisplay::SurfaceManager& surface_manager)
{
  ReaderCollection actions_collection;
  if (reader.read("actions", actions_collection)) {
    for (ReaderObject const& action_obj : actions_collection.get_objects()) {
      actions.push_back(parse_action(dir, action_obj.get_mapping(), surface_manager));
    }
  }

  if(actions.size() == 0) {
    throw std::runtime_error("Sprite contains no actions");
  }
}

SpriteAction*
SpriteData::parse_action(std::filesystem::path const& dir, ReaderMapping const& reader,
                         wstdisplay::SurfaceManager& surface_manager)
{
  std::unique_ptr<SpriteAction> action (new SpriteAction);
  action->speed = 1.0;
  action->scale = 1.0f;
  action->offset = glm::vec2(0, 0);

  reader.read("name", action->name);
  reader.read("speed", action->speed);
  reader.read("scale", action->scale);
  reader.read("offset", action->offset);

  ReaderMapping grid_reader;
  std::vector<std::string> image_files;
  if(reader.read("images", image_files))
  {
    //parse_images(action.get(), dir, images);

    for(std::vector<std::string>::const_iterator file = image_files.begin(); file != image_files.end(); ++file)
    {
      std::filesystem::path path = dir / *file;
      action->surfaces.push_back(surface_manager.get(path));
    }
  }
  else if(reader.read("image-grid", grid_reader))
  {
    std::string filename;
    int x_size = -1;
    int y_size = -1;

    grid_reader.read("file", filename);
    grid_reader.read("x-size", x_size);
    grid_reader.read("y-size", y_size);

    if(filename.empty() || x_size <= 0 || y_size <= 0)
      throw std::runtime_error("Invalid or too few data in image-grid");

    std::filesystem::path path = dir / filename;
    surface_manager.load_grid(path, action->surfaces, geom::isize(x_size, y_size));
  }

  if(action->name == "")
    throw std::runtime_error("No Name defined for action");

  if(action->surfaces.size() == 0)
  {
    std::ostringstream msg;
    msg << "Action '" << action->name << "' contains no images";
    throw std::runtime_error(msg.str());
  }
  return action.release();
}

} // namespace windstille

/* EOF */
