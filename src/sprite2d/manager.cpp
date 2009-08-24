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

#include "sprite2d/manager.hpp"


#include "sprite2d/data.hpp"

SpriteManager::SpriteManager()
  : datas()
{
}

SpriteManager::~SpriteManager()
{
}

SpriteDataPtr
SpriteManager::create_data(const Pathname& filename)
{
  Datas::iterator i = datas.find(filename);
  if(i != datas.end())
    {
      return i->second;
    }
  else
    {  
      SpriteDataPtr data(new SpriteData(filename));
      datas.insert(std::make_pair(filename, data));
      return data;
    }
}

void
SpriteManager::cleanup()
{
  for(Datas::iterator i = datas.begin(); i != datas.end(); ++i)
    {
      if (i->second.use_count() == 1)
        {
          datas.erase(i);
        }
    }
}

/* EOF */
