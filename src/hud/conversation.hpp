/*
**  Windstille - A Sci-Fi Action-Adventure Game
**  Copyright (C) 2000,2005 Ingo Ruhnke <grumbel@gmx.de>
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

#ifndef HEADER_WINDSTILLE_HUD_CONVERSATION_HPP
#define HEADER_WINDSTILLE_HUD_CONVERSATION_HPP

#include <vector>
#include <string>
#include "screen/screen.hpp"

/** Displays a list of text strings from which the user can select
    one, used to form multiple-choice dialogs */
class Conversation : public Screen
{
private:
  Vector2f pos;
  Vector2f direction;
  bool   active;
  int    selection;
  float  grow;
  float  time;

  struct Choice {
    Choice(const std::string topic_,
           const std::string text_)
      : topic(topic_), 
        text(text_) 
    {}
    std::string topic;
    std::string text;
  };

  typedef std::vector<Choice> Choices;
  Choices choices;
  
  static Conversation* current_;
public:
  static Conversation* current() { assert(current_); return current_; }
  
  Conversation();

  void draw();
  void update(float delta, const Controller& controller);

  void add(const std::string& text);
  void add(const std::string& topic, const std::string& text);
  int  get_selection() const;
  void show();
};

#endif

/* EOF */
