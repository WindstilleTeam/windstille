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

#ifndef HEADER_WINDSTILLE_HUD_SPEECH_MANAGER_HPP
#define HEADER_WINDSTILLE_HUD_SPEECH_MANAGER_HPP

#include <string>
#include <vector>

#include "math/vector2f.hpp"
#include "display/color.hpp"
#include "util/currenton.hpp"

class Entity;
class Speech;

/** The SpeechManager displays speech text over characters or
    objects, similar to what you see in LucasArts games */
class SpeechManager : public Currenton<SpeechManager>
{
private:
  typedef std::vector<Speech*> Speeches;
  Speeches speeches;
  int speech_id;

public:
  SpeechManager();
  ~SpeechManager();

  /** Add a speech bubble or a series of speech bubbles to the scene,
   *  lines seperated by newline are displayed in a series of speech
   *  bubbles.
   *
   *  @param text  The content of the speech bubble
   *  @param pos   The position of the speech bubble in screen coordinates
   *  @param color The color of the speech bubble
   */
  int add(const std::string& text,
          const Vector2f& pos,
          const Color& color = Color(1.0f, 1.0f, 1.0f));

  int add(const std::string& text,
          const Entity& entity,
          const Color& color = Color(1.0f, 1.0f, 1.0f));

  void draw();
  void update(float delta);

private:
  SpeechManager (const SpeechManager&);
  SpeechManager& operator= (const SpeechManager&);
};

#endif

/* EOF */
