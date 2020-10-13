/*
**  Windstille - A Sci-Fi Action-Adventure Game
**  Copyright (C) 2009 Ingo Ruhnke <grumbel@gmail.com>
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

#ifndef HEADER_WINDSTILLE_OBJECTS_DOLL_HPP
#define HEADER_WINDSTILLE_OBJECTS_DOLL_HPP

#include <memory>

#include <glm/glm.hpp>
#include "engine/game_object.hpp"
#include "util/currenton.hpp"

class Sprite3DDrawable;
namespace wstinput {
class Controller;
} // namespace wstinput
class EdgePosition;

namespace wstinput {
class Controller;
} // namespace wstinput

using Controller = wstinput::Controller;

/** A controllable character */
class Doll : public GameObject,
             public Currenton<Doll>
{
private:
  enum State {
    kNoState,
    kFalling,
    kWalking,
    kRunning,
    kStanding,
    kDucking,
    kRolling,
    kListing,
    kSwinging,
    kClimbing,
    kJumpUp
  };

  std::shared_ptr<Sprite3DDrawable> m_drawable;
  glm::vec2 m_velocity;
  glm::vec2 m_pos;
  glm::vec2 m_last_pos;
  std::unique_ptr<EdgePosition> m_edge_position;
  State m_state;

public:
  Doll();
  ~Doll() override;

  void draw (SceneContext& ) override {}
  void update (float delta) override;
  void update(const Controller& controller, float delta);

  glm::vec2 get_pos() const { return m_pos; }

private:
  void walk(const glm::vec2& adv);

  void set_state_falling();
  void set_state_standing();
  void set_state_walking();
  void set_state_running();
  void set_state_ducking();
  void set_state_jump_up();
  void set_state_rolling();
  void set_state_listing();
  void set_state_swinging();
  void set_state_climbing();
  void set_state_jump();

  void update_falling(const Controller& controller, float delta);
  void update_standing(const Controller& controller, float delta);
  void update_walking(const Controller& controller, float delta);
  void update_running(const Controller& controller, float delta);
  void update_ducking(const Controller& controller, float delta);
  void update_jump_up(const Controller& controller, float delta);
  void update_rolling(const Controller& controller, float delta);
  void update_listing(const Controller& controller, float delta);
  void update_swinging(const Controller& controller, float delta);
  void update_climbing(const Controller& controller, float delta);
  void update_jump(const Controller& controller, float delta);

private:
  Doll(const Doll&);
  Doll& operator=(const Doll&);
};

#endif

/* EOF */
