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

#ifndef HEADER_WINDSTILLE_SOUND_SOUND_SOURCE_HPP
#define HEADER_WINDSTILLE_SOUND_SOUND_SOURCE_HPP

#include <AL/al.h>
#include "math/vector.hpp"

class SoundSource
{
public:
  SoundSource();
  virtual ~SoundSource();

  void play();
  void stop();
  bool playing();

  void set_looping(bool looping);
  /// Set volume (0.0 is silent, 1.0 is normal)
  void set_gain(float gain);
  void set_position(const Vector& position);
  void set_velocity(const Vector& position);
  void set_reference_distance(float distance);

protected:
  friend class SoundManager;
  
  ALuint source;
};

#endif

/* EOF */
