/*
**  Windstille - A Sci-Fi Action-Adventure Game
**  Copyright (C) 2010 Ingo Ruhnke <grumbel@gmail.com>
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

#ifndef HEADER_WINDSTILLE_SOUND_DUMMY_SOUND_SOURCE_HPP
#define HEADER_WINDSTILLE_SOUND_DUMMY_SOUND_SOURCE_HPP

#include "sound/sound_source.hpp"

class DummySoundSource : public SoundSource
{
private:
public:
  DummySoundSource() {}
  ~DummySoundSource() override {}

  void play() override {}
  void stop() override {}
  bool is_playing() const override { return false; }

  float get_length() const override { return 0.0f; }

  void set_looping(bool looping) override {}

  /// Set volume (0.0 is silent, 1.0 is normal)
  void  set_gain(float gain) override {}
  float get_gain() const override { return 1.0f; }

  void  seek_to(float sec) override {}

  /** Return the current position in seconds */
  float get_pos() const override { return 0.0f; }

  int get_sample_pos() const override { return 0; }

  void set_position(const glm::vec2& position) override {}
  void set_velocity(const glm::vec2& position) override {}

  /** Distances closer then reference distance will increase the gain
      (or keep it at 1.0f), while distances further away will lower it */
  void set_reference_distance(float distance) override {}

  /** Higher factor will mean the sound gain lowers quicker with
      distance, while lower factor will reduce the lowering of gain,
      0.0f disables distance based reduction completly */
  void set_rolloff_factor(float factor) override {}

  /** Needs to be called whenever the SoundChannels gain changes */
  void update_gain() const override {}

  void update(float delta) override {}

private:
  DummySoundSource(const DummySoundSource&);
  DummySoundSource& operator=(const DummySoundSource&);
};

#endif

/* EOF */
