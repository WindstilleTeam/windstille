/*
**  Windstille - A Sci-Fi Action-Adventure Game
**  Copyright (C) 2009 Ingo Ruhnke <grumbel@gmx.de>
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

#ifndef HEADER_WINDSTILLE_EDITOR_SELECT_MASK_HPP
#define HEADER_WINDSTILLE_EDITOR_SELECT_MASK_HPP

#include <stdint.h>

class SelectMask
{
private:
  uint16_t mask;

public:
  SelectMask() : mask(~0) {}
  SelectMask(uint16_t mask_) : mask(mask_) {}

  uint16_t get_mask() const
  {
    return mask; 
  }

  bool match(const SelectMask& rhs) const
  {
    return (mask & rhs.mask);
  }

  void set(unsigned int layer, bool enable)
  {
    if (enable)
      {
        mask = static_cast<uint16_t>(mask | (1u<<layer));
      }
    else
      {
        mask = static_cast<uint16_t>(mask & (~(1u<<layer)));
      }
  }

  int size() const 
  {
    return 16; 
  }

  bool get(unsigned int layer) const 
  {
    return mask & (1<<layer);
  }

  operator bool() {
    return mask;
  }
};

#endif

/* EOF */
