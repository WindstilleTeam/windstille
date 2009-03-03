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

#ifndef HEADER_WINDSTILLE_HUD_SPEECH_MANAGER_HPP
#define HEADER_WINDSTILLE_HUD_SPEECH_MANAGER_HPP

#include <string>
#include <vector>
#include "screen/screen.hpp"
#include "math/vector2f.hpp"
#include "display/color.hpp"

class Entity;

/** The SpeechManager displays speech text over characters or
    objects, similar to what you see in LucasArts games */
class SpeechManager : public Screen
{
private:
  static SpeechManager* current_;
public:
  static SpeechManager* current() { return current_; }

private:
  class Speech 
  {
  public:
    std::string text;
    Vector2f    pos;
    Color       color;

    Speech(const std::string& text, const Vector2f& pos, const Color& color)
      : text(text),
        pos(pos),
        color(color)
    {}
    
    void draw();
    void update(float delta);
  };

  typedef std::vector<Speech*> Speeches;
  Speeches speeches;

public:
  SpeechManager();
  ~SpeechManager();

  void add(const std::string& text, const Vector2f& pos, const Color& color = Color(1.0f, 1.0f, 1.0f));

  void draw();
  void update(float delta, const Controller& controller);

private:
  SpeechManager (const SpeechManager&);
  SpeechManager& operator= (const SpeechManager&);
};

#endif

/* EOF */
