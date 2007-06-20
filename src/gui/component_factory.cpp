/*  $Id$
**   __      __ __             ___        __   __ __   __
**  /  \    /  \__| ____    __| _/_______/  |_|__|  | |  |   ____
**  \   \/\/   /  |/    \  / __ |/  ___/\   __\  |  | |  | _/ __ \
**   \        /|  |   |  \/ /_/ |\___ \  |  | |  |  |_|  |_\  ___/
**    \__/\  / |__|___|  /\____ /____  > |__| |__|____/____/\___  >
**         \/          \/      \/    \/                         \/
**  Copyright (C) 2005 Ingo Ruhnke <grumbel@gmx.de>
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

#include <stdexcept>
#include "tab_component.hpp"
#include "grid_component.hpp"
#include "automap.hpp"
#include "button.hpp"
#include "component_factory.hpp"

namespace gui {

ComponentFactory::ComponentFactory()
{
}

ComponentFactory::~ComponentFactory()
{
}

Component*
ComponentFactory::create(const std::string& name, FileReader& reader, Component* parent)
{
  if (name == "tab")
    {
      return new TabComponent(reader, parent);
    }
  else if (name == "automap")
    {
      return new Automap(reader, parent);
    }
  else if (name == "grid")
    {
      return new GridComponent(reader, parent);
    }
  else if (name == "button")
    {
      return new Button(reader, parent);
    }
  else
    {
      throw std::runtime_error("Error: Unknown component");
    }
}

} // namespace gui

/* EOF */
