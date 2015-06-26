/*
**  Windstille - A Sci-Fi Action-Adventure Game
**  Copyright (C) 2009 Ingo Ruhnke <grumbel@gmail.com>
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

#include "objects/layer.hpp"

#include <stdexcept>

#include "engine/sector.hpp"
#include "engine/script_manager.hpp"

Layer::Layer(const FileReader& reader) :
  objects(),
  new_objects()
{
#if 0
  FileReader objects_reader;
  if(reader.get("objects", objects_reader) == false)
  {
    throw std::runtime_error("No objects specified");
  }
  else
  {
    std::vector<FileReader> objects_readers = objects_reader.get_sections();
    for(std::vector<FileReader>::iterator i = objects_readers.begin(); i != objects_readers.end(); ++i)
    {
      Sector::current()->add_object(*i);
    }
  }
#endif
}

Layer::~Layer()
{
}

void
Layer::commit_adds()
{
  if (!new_objects.empty())
  {
    // Add new game objects
    for(Objects::iterator i = new_objects.begin(); i != new_objects.end(); ++i) {
      objects.push_back(*i);
    }
    new_objects.clear();
  }
}

void
Layer::commit_removes()
{
  // remove objects
  for(Objects::iterator i = objects.begin(); i != objects.end(); )
  {
    boost::shared_ptr<GameObject>& object = *i;

    if(object->is_removable())
    {
      if(object->get_name() != "")
      {
        ScriptManager::current()->remove_object_from_squirrel(object);
      }

      i = objects.erase(i);
      continue;
    }

    ++i;
  }
}

void
Layer::draw (SceneContext& /*context*/)
{
  // do nothing
}

void
Layer::update (float delta)
{
  commit_adds();

  for(Objects::iterator i = objects.begin(); i != objects.end(); ++i)
  {
    if ((*i)->is_active())
      (*i)->update(delta);
  }

  commit_removes();
}

/* EOF */
