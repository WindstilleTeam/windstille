/*
**  Windstille - A Sci-Fi Action-Adventure Game
**  Copyright (C) 2000,2005 Ingo Ruhnke <grumbel@gmail.com>
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

#ifndef HEADER_WINDSTILLE_ENGINE_CAMERA_HPP
#define HEADER_WINDSTILLE_ENGINE_CAMERA_HPP

#include <vector>

#include <glm/glm.hpp>
#include "util/currenton.hpp"

/**
 * This class manages the virtual camera movement, it follows the
 * player, allows the player to watch around, might zoom out if
 * interesting stuff happens out of the screen and such
 */
class Camera : public Currenton<Camera>
{
public:
  enum Mode { CAMERA_INACTIVE, CAMERA_FOLLOW_PLAYER, CAMERA_FOLLOW_PATH };

  struct PathPoint
  {
    glm::vec2 pos;
    float  zoom;
    float  rotation;

    PathPoint(const glm::vec2& pos_, float zoom_, float rotation_ = 0.0f)
      : pos(pos_), zoom(zoom_), rotation(rotation_) {}

    bool operator==(const PathPoint& p) const {
      return p.pos == pos && p.zoom == zoom;
    }
  };

private:
  Mode mode;

  /**
   * Position of the camera, ie. where the center of the screen should
   * be located
   */
  glm::vec2 pos;

  /**
   * Zoom of the Camera, 1.0 means no zoom, 2.0 means everything will
   * be twice as large
   */
  float  zoom;

  std::vector<PathPoint> path;
  float path_pos;

public:
  Camera();

  void   update(float delta);

  glm::vec2 get_pos() const { return pos; }
  void   set_pos(float x, float y);

  void   set_zoom(float zoom_);
  float  get_zoom() const;

  void   set_mode(Mode mode_);

  /**
   * Set Camera to follow the given path
   *
   * @param path_ The path the camera should follow
   * @param cont True if you want to continue the path from the
   * current camera position, false if you want the camera to jump to
   * the start of the path instantly
   */
  void   set_path(const std::vector<PathPoint>& path_, bool cont);

private:
  Camera (const Camera&);
  Camera& operator= (const Camera&);
};

#endif

/* EOF */
