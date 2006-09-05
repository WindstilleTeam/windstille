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

#ifndef HEADER_PARTICLE_VIEWER_HPP
#define HEADER_PARTICLE_VIEWER_HPP

#include "screen.hpp"
#include "sprite2d/sprite.hpp"
#include "gui/gui_manager.hpp"
#include "particles/particle_system.hpp"

namespace GUI {
class Slider;
class TabComponent;
}

class ParticleSystemGUI;

/** */
class ParticleViewer : public Screen
{
private:
  SceneContext sc;
  typedef std::vector<ParticleSystem*> Systems;
  Systems systems;
  Sprite background;

  Vector pos;
  bool   show_gui;
  GUI::GUIManager* manager;
  GUI::TabComponent* tab;

  typedef std::vector<ParticleSystemGUI*> ParticleSystemGUIs;
  ParticleSystemGUIs guis;
public:
  ParticleViewer();
  ~ParticleViewer();
  
  void draw();
  void update(float delta, const Controller& controller);
  void load(const std::string& filename);

private:
  ParticleViewer (const ParticleViewer&);
  ParticleViewer& operator= (const ParticleViewer&);
};

#endif

/* EOF */
