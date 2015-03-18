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

#ifndef HEADER_WINDSTILLE_SPRITE2D_MANAGER_HPP
#define HEADER_WINDSTILLE_SPRITE2D_MANAGER_HPP

#include <map>
#include <boost/shared_ptr.hpp>

#include "util/currenton.hpp"

class Pathname;
class SpriteData;

typedef boost::shared_ptr<SpriteData> SpriteDataPtr;

class SpriteManager : public Currenton<SpriteManager>
{
public:
  SpriteManager();
  ~SpriteManager();

  SpriteDataPtr create_data(const Pathname& filename);

  /** Removes all cached Sprites that are no longer in use */
  void cleanup();
private:
  typedef std::map<Pathname, SpriteDataPtr> Datas;
  Datas datas;
};

#endif

/* EOF */
