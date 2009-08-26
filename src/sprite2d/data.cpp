/*
**  Windstille - A Sci-Fi Action-Adventure Game
**  Copyright (C) 2005,2007 Matthias Braun <matze@braunis.de>,
**                          Ingo Ruhnke <grumbel@gmx.de>
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

#include <stdexcept>

#include "display/surface_manager.hpp"

SpriteData::SpriteData(const Pathname& pathname)
  : actions()
{
  if (pathname.exists())
  {
    const std::string ext = pathname.get_extension();

    if (ext == "sprite")
    {
      FileReader reader = FileReader::parse(pathname);

      if(reader.get_name() != "sprite") {
        std::ostringstream msg;
        msg << "File " << pathname << " is not a windstille sprite";
        throw std::runtime_error(msg.str());
      }
    
      parse(pathname.get_dirname(), reader);
    }
    else if (ext == "png" || ext == "jpg")
    {
      std::auto_ptr<SpriteAction> action(new SpriteAction());
      action->name   = "default";
      action->speed  = 1.0;
      action->scale  = 1.0f;
      action->offset = Vector2f(0, 0);
      action->surfaces.push_back(Surface(pathname));
      actions.push_back(action.release());
    }
    else
    {
      std::ostringstream str;
      str << "Sprite " << pathname << " has unknown suffix: '" << ext  << "'";
      throw std::runtime_error(str.str());
    }
  }
  else if (pathname.get_raw_path().length() > std::string(".sprite").length())
  { // If sprite file is not found, we search for a file with the
    // same name ending in .png
    Pathname pngfile(pathname.get_raw_path().substr(0, pathname.get_raw_path().length() - std::string(".sprite").length()) + ".png",
                     pathname.get_type());

    if (pngfile.exists())
    {
      std::auto_ptr<SpriteAction> action(new SpriteAction);
      action->name   = "default";
      action->speed  = 1.0;
      action->scale  = 1.0f;
      action->offset = Vector2f(0, 0);
      action->surfaces.push_back(Surface(pngfile));
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
    str << "Couldn't find " << pathname;
    throw std::runtime_error(str.str());
  }
}

SpriteData::~SpriteData()
{
  for(Actions::iterator i = actions.begin(); i != actions.end(); ++i)
    delete *i;
}

void
SpriteData::parse(const Pathname& dir, FileReader& reader)
{
  std::vector<FileReader> sections = reader.get_sections();
  for(std::vector<FileReader>::iterator i = sections.begin(); i != sections.end(); ++i)
    actions.push_back(parse_action(dir, *i));
  
  if(actions.size() == 0)
    throw std::runtime_error("Sprite contains no actions");
}

SpriteAction*
SpriteData::parse_action(const Pathname& dir, FileReader& reader)
{
  std::auto_ptr<SpriteAction> action (new SpriteAction);
  action->speed = 1.0;
  action->scale = 1.0f;
  action->offset = Vector2f(0, 0);
 
  reader.get("name", action->name);
  reader.get("speed", action->speed);
  reader.get("scale", action->scale);
  reader.get("offset", action->offset);
  
  FileReader grid_reader;
  std::vector<std::string> image_files;
  if(reader.get("images", image_files))
    {
      //parse_images(action.get(), dir, images);

      for(std::vector<std::string>::iterator file = image_files.begin(); file != image_files.end(); ++file)
        {
          Pathname path = dir;
          path.append_path(*file);
          action->surfaces.push_back(SurfaceManager::current()->get(path));
        }
    }
  else if(reader.get("image-grid", grid_reader)) 
    {
      std::string filename;
      int x_size = -1;
      int y_size = -1;
      
      grid_reader.get("file", filename);
      grid_reader.get("x-size", x_size);
      grid_reader.get("y-size", y_size);

      if(filename.empty() || x_size <= 0 || y_size <= 0)
        throw std::runtime_error("Invalid or too few data in image-grid");
      
      Pathname path = dir;
      path.append_path(filename);
      SurfaceManager::current()->load_grid(path, action->surfaces, x_size, y_size);
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
 
/* EOF */
