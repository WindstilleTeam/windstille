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

#include <GL/glew.h>
#include <filesystem>
#include <iostream>

#include <wstinput/controller.hpp>

#include "app/app.hpp"
#include "app/controller_def.hpp"
#include "armature/pose.hpp"
#include <wstdisplay/graphics_context.hpp>
#include "screen/armature_test.hpp"
#include "screen/screen_manager.hpp"
#include "util/directory.hpp"
#include "util/pathname.hpp"

ArmatureTest::ArmatureTest()
  : model(),
    armature(),
    poses(),
    pose_idx(0),
    time(0.0f),
    xrot(0.0f),
    yrot(0.0f),
    zrot(0.0f)
{
  ReaderDocument const& model_doc = ReaderDocument::from_file(Pathname("armature/mesh.mesh").get_sys_path());
  model = std::make_unique<Model>(model_doc, Pathname("armature/"), g_app.texture());

  ReaderDocument const& armature_doc = ReaderDocument::from_file(Pathname("armature/armature.arm").get_sys_path());
  armature.reset(new Armature(armature_doc));

  {
    Directory::List file_lst = Directory::read(Pathname("armature/pose/"));

    for(Directory::List::iterator i = file_lst.begin(); i != file_lst.end(); ++i)
    {
      if (!std::filesystem::is_directory(i->get_sys_path()))
      {
        std::cout << "PoseFile: " << *i << std::endl;
        ReaderDocument pose_doc = ReaderDocument::from_file(i->get_sys_path());
        poses.push_back(new Pose(pose_doc));
      }
    }
  }

  pose_idx = 0;

  armature->apply(*poses[pose_idx]);

  xrot = 0;
  yrot = 0;
  zrot = 0;

  time = 0.0f;
}

ArmatureTest::~ArmatureTest()
{
  for(std::vector<Pose*>::iterator i = poses.begin(); i != poses.end(); ++i)
    delete *i;
  poses.clear();
}

void
ArmatureTest::draw(wstdisplay::GraphicsContext& gc)
{
  glClearColor(0.5f, 0.0f, 0.5f, 1.0f);
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

  gc.push_matrix();

  gc.translate(400.0f, 300.0f, 0.0f);
  gc.scale(64.0f, 64.0f, 64.0f);

  gc.rotate(xrot, 1.0f, 0.0f, 0.0f);
  gc.rotate(yrot, 0.0f, 1.0f, 0.0f);
  gc.rotate(zrot, 0.0f, 0.0f, 1.0f);

  model->draw(gc);
  armature->draw(gc);

  gc.pop_matrix();

  // std::cout << xrot << " " << yrot << std::endl;
}

void
ArmatureTest::update(float delta, const Controller& controller)
{
  time += delta;

  pose_idx = int(time * 5.0f) % static_cast<int>(poses.size());
  armature->apply(*poses[pose_idx]);
  model->apply(armature.get());

  if (controller.button_was_pressed(ESCAPE_BUTTON) ||
      controller.button_was_pressed(PAUSE_BUTTON))
  {
    ScreenManager::current()->pop_screen();
  }

  if (controller.button_was_pressed(PRIMARY_BUTTON))
  {
    xrot = 90;
    yrot = 0;
    zrot = 0;
  }
  else if (controller.button_was_pressed(SECONDARY_BUTTON))
  {
    xrot = 0;
    yrot = 90;
    zrot = 0;
  }
  else if (controller.button_was_pressed(TERTIARY_BUTTON))
  {
    xrot = 0;
    yrot = 0;
    zrot = 90;
  }
  else
  {
    yrot += controller.get_axis_state(X_AXIS) * 90 * delta;
    xrot += controller.get_axis_state(Y_AXIS) * 90 * delta;
    zrot += controller.get_axis_state(X2_AXIS) * 90 * delta;
  }

  if (controller.button_was_pressed(AIM_BUTTON))
  {
  }
}

/* EOF */
