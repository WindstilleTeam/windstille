/*
**  Windstille - A Sci-Fi Action-Adventure Game
**  Copyright (C) 2007 Ingo Ruhnke <grumbel@gmail.com>
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

#ifndef HEADER_WINDSTILLE_SCREEN_ARMATURE_TEST_HPP
#define HEADER_WINDSTILLE_SCREEN_ARMATURE_TEST_HPP

#include <memory>

#include "armature/armature.hpp"
#include "armature/model.hpp"
#include "screen/screen.hpp"

/** */
class ArmatureTest : public Screen
{
private:
  std::unique_ptr<Model>    model;
  std::unique_ptr<Armature> armature;
  std::vector<Pose*> poses;

  int pose_idx;
  float time;

  float xrot;
  float yrot;
  float zrot;

public:
  ArmatureTest();
  ~ArmatureTest() override;

  void draw(GraphicsContext& gc) override;
  void update(float delta, const Controller& controller) override;

private:
  ArmatureTest (const ArmatureTest&);
  ArmatureTest& operator= (const ArmatureTest&);
};

#endif

/* EOF */
