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

#ifndef HEADER_WINDSTILLE_SPRITE3D_MANAGER_HPP
#define HEADER_WINDSTILLE_SPRITE3D_MANAGER_HPP

#include "util/pathname.hpp"
#include "util/currenton.hpp"

namespace sprite3d {

class Data;

class Manager : public Currenton<Manager>
{
public:
  Manager();
  ~Manager() override;

  Data* create_data(std::filesystem::path const& filename);

private:
  typedef std::map<std::filesystem::path, Data*> Datas;
  Datas datas;
};

} // namespace sprite3d

#endif

/* EOF */
