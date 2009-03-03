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

#include "font/fonts.hpp"
#include "speech_manager.hpp"

SpeechManager* SpeechManager::current_ = 0;

void 
SpeechManager::Speech::draw()
{
  Fonts::vera20->draw(pos, text, color);
}

void
SpeechManager::Speech::update(float delta)
{
}

SpeechManager::SpeechManager()
{
  assert(current_ == 0);
  current_ = this;

  add("Hello World", Vector2f(320, 200));
}

SpeechManager::~SpeechManager()
{
  for(Speeches::iterator i= speeches.begin(); i != speeches.end(); ++i)
    delete *i;
  speeches.clear();
}

void
SpeechManager::add(const std::string& text, const Vector2f& pos, const Color& color)
{
  speeches.push_back(new Speech(text, pos, color));
}

void
SpeechManager::draw()
{
  for(Speeches::iterator i= speeches.begin(); i != speeches.end(); ++i)
    {
      (*i)->draw();
    }  
}

void
SpeechManager::update(float delta, const Controller& controller)
{
  for(Speeches::iterator i= speeches.begin(); i != speeches.end(); ++i)
    {
      (*i)->update(delta);
    }
}

/* EOF */
