/*
**  Windstille - A Sci-Fi Action-Adventure Game
**  Copyright (C) 2005 Matthias Braun <matze@braunis.de>
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

#include "sprite3d/manager.hpp"

#include "sprite3d/data.hpp"
#include "sprite3d/sprite3d.hpp"

namespace sprite3d {

Manager::Manager()
{
}

Manager::~Manager()
{
  for(Datas::iterator i = datas.begin(); i != datas.end(); ++i)
    delete i->second;
}

Data*
Manager::create_data(const Pathname& filename)
{
  Datas::iterator i = datas.find(filename);
  if(i != datas.end())
    return i->second;
  
  Data* data = new Data(filename);
  datas.insert(std::make_pair(filename, data));
  return data;
}

} // namespace sprite3d

/* EOF */
