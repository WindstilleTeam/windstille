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

#ifndef HEADER_WINDSTILLE_SPRITE3D_SPRITE3D_HPP
#define HEADER_WINDSTILLE_SPRITE3D_SPRITE3D_HPP

#include <string>
#include <stdint.h>

#include <glm/glm.hpp>

#include <wstdisplay/scene_context.hpp>
#include <wstdisplay/fwd.hpp>

#include "sprite3d/data.hpp"

namespace windstille {

namespace sprite3d {

class Data;
struct Action;
struct ActionFrame;
class Manager;

} // namespace sprite3d

/**
 * This class is a 3d sprite. It's a set of textured meshs with different
 * animations (called actions) that are keyframe animated.
 */
class Sprite3D
{
public:
  typedef uint16_t PointID;

  Sprite3D();
  explicit Sprite3D(std::filesystem::path const& filename, sprite3d::Manager& sprite3d_manager);
  Sprite3D(Sprite3D const&);
  Sprite3D& operator=(Sprite3D const&);
  ~Sprite3D();

  /**
   * You should call this every frame
   */
  void update(float delta);
  void draw(wstdisplay::DrawingContext& sc, glm::vec2 const& pos, float z_pos);

  /**
   * Changes action (after the currently shown animation frame)
   */
  void set_action(std::string const& name, float speed = 1.0);

  /**
   * Return the name of the currently active action
   */
  std::string const& get_action() const;

  /**
   * Return a list of all available actions
   */
  std::vector<std::string> get_actions() const;

  /**
   * Return a list of all available attachment points
   */
  std::vector<std::string> get_attachment_points() const;

  /**
   * Set the next action and vflip value to be played after the current action
   * has finished (or reached the point to be defined by abort_at_marker)
   */
  void set_next_action(std::string const& name, float speed = 1.0);
  void set_next_rot(bool rot);

  /**
   * Abort current action after a certain marker has been reached.
   * (This only works if set_next_action has been used before)
   */
  void abort_at_marker(std::string const& marker);

  /**
   * returns true if the current frame of the action is before a certain marker
   */
  bool before_marker(std::string const& marker) const;

  /**
   * returns true exactly once after actions have been switched after a
   * set_next_action call (another call to set_next_action resets this flag)
   */
  bool switched_actions();

  /**
   * The speed of the action is multiplied with this factor
   */
  void  set_speed(float speed);
  float get_speed() const;

  /**
   * Rotate (or not rotate) the model 180 degree
   */
  void set_rot(bool rot = true);
  bool get_rot() const;

  PointID get_attachment_point_id(std::string const& name) const;
  glm::mat4  get_attachment_point_matrix(PointID id) const;

  /** true if the Sprite3D is valid and usable, false if not */
  bool is_valid() const;

  void set_blend_func(GLenum sfactor, GLenum dfactor);

private:
  friend class Sprite3DDrawable;
  void draw(wstdisplay::GraphicsContext& gc, glm::vec2 const& pos, glm::mat4 const& modelview);

  struct Frame {
    sprite3d::Action const* action;
    int   frame;
    float speed;
    bool  rot;

    bool operator==(Frame const& o) const
    {
      return action == o.action && frame == o.frame && speed == o.speed
        && rot == o.rot;
    }
  };

  void set_next_frame();

private:
  sprite3d::Data const* data;
  bool actions_switched;

  Frame frame1;
  Frame frame2;
  float blend_time;

  Frame next_frame;
  Frame next_action;
  Frame abort_at_frame;

  GLenum blend_sfactor;
  GLenum blend_dfactor;
};

} // namespace windstille

#endif

/* EOF */
