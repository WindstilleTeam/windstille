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

#include "app/app.hpp"
#include "engine/entity.hpp"
#include "engine/script_manager.hpp"
#include "font/fonts.hpp"
#include "hud/speech_manager.hpp"

namespace windstille {


class Speech
{
public:
  int id;
  std::string text;
  glm::vec2    pos;
  surf::Color       color;
  float       seconds_passed;
  float       seconds_till_done;

  Speech(int id, std::string const& text, glm::vec2 const& pos, surf::Color const& color);

  void draw(wstdisplay::GraphicsContext& gc);
  void update(float delta);
  bool is_done() const;
};


Speech::Speech(int id_, std::string const& text_, glm::vec2 const& pos_, surf::Color const& color_)
  : id(id_),
    text(text_),
    pos(pos_),
    color(color_),
    seconds_passed(0.0f),
    seconds_till_done()
{
  // FIXME: Calculate words by other means, this isn't good for long text
  float words = 2.0f + static_cast<float>(text.size()) / 5.0f;
  float words_per_minute = 150.0f;
  float words_per_second = words_per_minute / 60.0f;

  seconds_till_done = words / words_per_second;
}

void
Speech::draw(wstdisplay::GraphicsContext& gc)
{
  // Do not display any text for 0.1 seconds before we are doen, so
  // that we get an empty gap between succesive text on the screen,
  // which is needed to make text look more like natural speech.
  if (seconds_passed < (seconds_till_done - .1f)) {
    g_app.fonts().vera20->draw_center(gc, pos, text, color);
  }
}

void
Speech::update(float delta)
{
  seconds_passed += delta;
}

bool
Speech::is_done() const
{
  return seconds_passed > seconds_till_done;
}


SpeechManager::SpeechManager() :
  speeches(),
  speech_id(1)
{
}

SpeechManager::~SpeechManager()
{
  for(Speeches::iterator i= speeches.begin(); i != speeches.end(); ++i)
    delete *i;
  speeches.clear();
}

int
SpeechManager::add(std::string const& text, Entity const& entity, surf::Color const& color)
{
  return add(text, entity.get_pos(), color);
}

int
SpeechManager::add(std::string const& text, glm::vec2 const& pos, surf::Color const& color)
{
  int this_speech_id = speech_id;
  speeches.push_back(new Speech(this_speech_id, text, pos, color));
  speech_id += 1;
  return this_speech_id;
}

void
SpeechManager::draw(wstdisplay::GraphicsContext& gc)
{
  for(Speeches::iterator i= speeches.begin(); i != speeches.end(); ++i)
  {
    if (*i) {
      (*i)->draw(gc);
    }
  }
}

void
SpeechManager::update(float delta)
{
  for(Speeches::iterator i= speeches.begin(); i != speeches.end(); ++i)
  {
    if (*i)
      (*i)->update(delta);
  }

  for(Speeches::iterator i= speeches.begin(); i != speeches.end(); ++i)
  {
    if ((*i)->is_done())
    {
      ScriptManager::WakeupData event;
      event.type = ScriptManager::SPEECH_DONE;
      event.id   = (*i)->id;
      ScriptManager::current()->fire_wakeup_event(event);

      delete *i;
      *i = nullptr;
    }
  }

  speeches.erase(std::remove(speeches.begin(), speeches.end(), static_cast<Speech*>(nullptr)),
                 speeches.end());
}


} // namespace windstille

/* EOF */
