/*
**  Windstille - A Sci-Fi Action-Adventure Game
**  Copyright (C) 2005 Ingo Ruhnke <grumbel@gmail.com>
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

#ifndef HEADER_WINDSTILLE_DISPLAY_TEXT_AREA_HPP
#define HEADER_WINDSTILLE_DISPLAY_TEXT_AREA_HPP

#include <memory>
#include <string>

#include <geom/geom.hpp>
#include <wstdisplay/fwd.hpp>

class TextAreaImpl;

/** A class for managing multiple lines of text in combination with
    special formating.
*/
class TextArea
{
public:
  /** The area which the TextArea should cover */
  TextArea(const geom::frect& rect, bool letter_by_letter);
  ~TextArea();

  /** Set the rectangle into which the TextArea should fill its text */
  void set_rect(const geom::frect& rect);

  /** Sets the text to be displayed in the text box */
  void set_text(const std::string& str);

  /** Sets the default font to be used in the text box, can be changed
      with special tags */
  void set_font(wstdisplay::TTFFont* font);

  /** Skips letter by letter display */
  void set_progress_complete();

  bool is_progress_complete();

  void update(float delta);
  void draw(wstdisplay::GraphicsContext& gc);

  /** Return the rectangle that TextArea fills its text into */
  geom::frect get_rect() const;

  /** Return the position of the last character in the TextArea,
      usefull to display a blinking cursor or things like that */
  glm::vec2 get_cursor_pos() const;

  float get_scroll_offset() const;
  void  set_scroll_offset(float s);

private:
  std::unique_ptr<TextAreaImpl> impl;
};

#endif

/* EOF */
