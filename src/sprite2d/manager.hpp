/*  $Id: windstille.hpp 1460 2007-06-18 04:03:31Z grumbel $
**   __      __ __             ___        __   __ __   __
**  /  \    /  \__| ____    __| _/_______/  |_|__|  | |  |   ____
**  \   \/\/   /  |/    \  / __ |/  ___/\   __\  |  | |  | _/ __ \
**   \        /|  |   |  \/ /_/ |\___ \  |  | |  |  |_|  |_\  ___/
**    \__/\  / |__|___|  /\____ /____  > |__| |__|____/____/\___  >
**         \/          \/      \/    \/                         \/
**  Copyright (C) 2005,2007 Matthias Braun <matze@braunis.de>,
**                          Ingo Ruhnke <grumbel@gmx.de>
**
**  This program is free software; you can redistribute it and/or
**  modify it under the terms of the GNU General Public License
**  as published by the Free Software Foundation; either version 2
**  of the License, or (at your option) any later version.
**
**  This program is distributed in the hope that it will be useful,
**  but WITHOUT ANY WARRANTY; without even the implied warranty of
**  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
**  GNU General Public License for more details.
** 
**  You should have received a copy of the GNU General Public License
**  along with this program; if not, write to the Free Software
**  Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA
**  02111-1307, USA.
*/

#ifndef __SPRITE2D_MANAGER_HPP__
#define __SPRITE2D_MANAGER_HPP__

#include <string>
#include <map>
#include <boost/shared_ptr.hpp>

class SpriteData;
typedef boost::shared_ptr<SpriteData> SpriteDataPtr;

class SpriteManager
{
public:
  SpriteManager();
  ~SpriteManager();

  SpriteDataPtr create_data(const std::string& filename);
  
  /** Removes all cached Sprites that are no longer in use */
  void cleanup();
private:
  typedef std::map<std::string, SpriteDataPtr> Datas;
  Datas datas;
};

extern SpriteManager* sprite2d_manager;

#endif

