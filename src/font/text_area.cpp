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

#include "font/text_area.hpp"

#include <babyxml.hpp>

#include "app/app.hpp"
#include "display/graphics_context.hpp"
#include "font/fonts.hpp"
#include "scenegraph/vertex_array_drawable.hpp"

struct TextAreaCommand
{
  enum Type { WORD, START, END } type;
  std::string content;

  TextAreaCommand(Type type_, const std::string& content_)
    : type(type_),
      content(content_)
  {}
};

class TextAreaImpl
{
public:
  TTFFont* font;
  geom::frect rect;
  float passed_time;

  int v_space;
  bool letter_by_letter;
  bool progress_complete;
  std::vector<TextAreaCommand> commands;
  glm::vec2 cursor_pos;
  float scroll_offset;
  float max_scroll_offset;

  TextAreaImpl()
    : font(),
      rect(),
      passed_time(),
      v_space(),
      letter_by_letter(),
      progress_complete(),
      commands(),
      cursor_pos(),
      scroll_offset(),
      max_scroll_offset()
  {}

private:
  TextAreaImpl(const TextAreaImpl&);
  TextAreaImpl& operator=(const TextAreaImpl&);
};

TextArea::TextArea(const geom::frect& rect, bool letter_by_letter) :
  impl(new TextAreaImpl)
{
  impl->font = g_app.fonts().vera20.get();
  impl->rect    = rect;
  // FIXME: freetype might provide info for vspacing, not sure
  impl->v_space = 2;
  impl->letter_by_letter = letter_by_letter;
  impl->progress_complete = false;
  impl->passed_time       = 0;
  impl->scroll_offset     = 0.0f;
  impl->max_scroll_offset = -1.0f; // FIXME: Bad magick value
}

TextArea::~TextArea()
{
}

void
TextArea::set_rect(const geom::frect& rect)
{
  impl->rect = rect;
}

void
TextArea::set_text(const std::string& str)
{
  impl->scroll_offset     = 0.0f;
  impl->max_scroll_offset = -1.0f;

  impl->commands.clear();

  for(auto const& node : babyxml::parse(str))
  {
    if (node.type == babyxml::NodeType::START_TAG)
    {
      impl->commands.push_back(TextAreaCommand(TextAreaCommand::START, node.content));
    }
    else if (node.type == babyxml::NodeType::END_TAG)
    {
      impl->commands.push_back(TextAreaCommand(TextAreaCommand::END, node.content));
    }
    else if (node.type == babyxml::NodeType::TEXT)
    {
      // Seperate the given str into words, words are seperated by either
      // ' ' or '\n', space is threaded as a word of its own
      // "Hello  World \n" => ("Hello", " ", " ", "World", " ", "\n")
      std::string word;
      for(std::string::const_iterator j = node.content.begin(); j != node.content.end(); ++j)
      {
        if (*j == ' ' || *j == '\n')
        {
          if (word.empty())
          {
            impl->commands.push_back(TextAreaCommand(TextAreaCommand::WORD, std::string(1, *j)));
          }
          else
          {
            impl->commands.push_back(TextAreaCommand(TextAreaCommand::WORD, word));
            impl->commands.push_back(TextAreaCommand(TextAreaCommand::WORD, std::string(1, *j)));
            word = "";
          }
        }
        else
        {
          word += *j;
        }
      }
      if (!word.empty())
        impl->commands.push_back(TextAreaCommand(TextAreaCommand::WORD, word));
    }
  }
}

void
TextArea::set_font(TTFFont* font)
{
  impl->font = font;
}

void
TextArea::set_progress_complete()
{
  impl->letter_by_letter = false;
}

bool
TextArea::is_progress_complete()
{
  return impl->progress_complete;
}

void
TextArea::draw(GraphicsContext& gc)
{
  assert(impl->font);
  if (impl->max_scroll_offset > 0.0f)
  {
    float height = impl->max_scroll_offset + impl->rect.height();

    gc.fill_rounded_rect(geom::frect(glm::vec2(impl->rect.right() + 4,
                                               impl->rect.top() + impl->scroll_offset*impl->rect.height()/height),
                                     geom::fsize(8, impl->rect.height()*impl->rect.height()/height)),
                         4.0f, Color(1.0f, 1.0f, 1.0f, 0.25f));
  }
  VertexArrayDrawable va;

  va.set_texture(impl->font->get_texture());

  va.set_blend_func(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

  gc.push_matrix();
  gc.translate(impl->rect.left(),
               impl->rect.top() + static_cast<float>(impl->font->get_height()) - impl->scroll_offset,
               0.0f);

  va.set_mode(GL_QUADS);

  int x_pos = 0;
  int y_pos = 0;

  Color top_color    = Color(1.0f, 1.0f, 1.0f);
  Color bottom_color = Color(1.0f, 1.0f, 1.0f);
  bool is_small = false;
  bool is_large = false;
  float eat_time = impl->passed_time;
  bool sinus = false;
  bool break_writing = false;

  std::vector<TextAreaCommand>::const_iterator i = impl->commands.begin();
  for(; i != impl->commands.end(); ++i)
  {
    if (break_writing)
      break;

    switch (i->type)
    {
      case TextAreaCommand::START:
        if (i->content == "b")
        {
          top_color    = Color(1.0f, 0.0f, 0.0f);
          bottom_color = Color(0.8f, 0.0f, 0.0f);
        }
        else if (i->content == "i")
        {
          top_color    = Color(0.65f, 0.7f, 1.0f);
          bottom_color = Color(0.65f, 0.7f, 1.0f);
        }
        else if (i->content == "small")
        {
          is_small = true;
        }
        else if (i->content == "large")
        {
          is_large = true;
        }
        else if (i->content == "sleep")
        {
          eat_time -= 1.0f;
        }
        else if (i->content == "sin")
        {
          sinus = true;
        }
        break;

      case TextAreaCommand::END:
        if (i->content == "b" || i->content == "i")
        {
          top_color    = Color(1.0f, 1.0f, 1.0f);
          bottom_color = Color(1.0f, 1.0f, 1.0f);
        }
        else if (i->content == "small")
        {
          is_small = false;
        }
        else if (i->content == "large")
        {
          is_large = false;
        }
        else if (i->content == "sin")
        {
          sinus = false;
        }
        break;

      case TextAreaCommand::WORD:
        retry:
        int word_width;

        if (is_small)
          word_width = static_cast<int>(static_cast<float>(impl->font->get_width(i->content)) * 0.6f);
        else if (is_large)
          word_width = static_cast<int>(static_cast<float>(impl->font->get_width(i->content)) * 2.0f);
        else
          word_width = impl->font->get_width(i->content);

        if (i->content == "\n")
        {
          x_pos = 0;
          y_pos += impl->font->get_height() + impl->v_space;
        }
        else if (static_cast<float>(x_pos + word_width) > impl->rect.width() && static_cast<float>(word_width) <= impl->rect.width())
        {
          x_pos = 0;
          y_pos += impl->font->get_height() + impl->v_space;
          goto retry;
        }
        else
        {
          if (x_pos == 0 && i->content == " ")
          {
            // ignore space at the beginning of a line
          }
          else
          {
            for(std::string::const_iterator j = i->content.begin(); j != i->content.end(); ++j)
            {
              if (impl->letter_by_letter && eat_time <= 0)
              {
                break_writing = true;
                break;
              }

              float x = static_cast<float>(x_pos);
              float y = static_cast<float>(y_pos);

              if (sinus) // FIXME: this could actually work per vertex
                y += sinf(impl->passed_time * 10.0f + static_cast<float>(x_pos) / 15.0f) * 5.0f;

              if (*j == '.' || *j == '\n')
                eat_time -= 0.50f;
              else
                eat_time -= 0.05f;

              const TTFCharacter& character = impl->font->get_character(*j);

              bool draw_it = (static_cast<float>(y_pos) >= impl->scroll_offset &&
                              static_cast<float>(y_pos) < impl->scroll_offset + impl->rect.height() - static_cast<float>(impl->font->get_height()));
              if (is_small)
              {
                float scale = 0.6f;
                if (draw_it)
                {
                  va.color(top_color);
                  va.texcoord(character.uv.left(), character.uv.top());
                  va.vertex(x + scale * static_cast<float>(character.pos.left()),
                             y + scale * static_cast<float>(character.pos.top()));

                  va.color(top_color);
                  va.texcoord(character.uv.right(), character.uv.top());
                  va.vertex(x + scale * static_cast<float>(character.pos.right()),
                             y + scale * static_cast<float>(character.pos.top()));

                  va.color(bottom_color);
                  va.texcoord(character.uv.right(), character.uv.bottom());
                  va.vertex(x + scale * static_cast<float>(character.pos.right()),
                             y + scale * static_cast<float>(character.pos.bottom()));

                  va.color(bottom_color);
                  va.texcoord(character.uv.left(), character.uv.bottom());
                  va.vertex(x + scale * static_cast<float>(character.pos.left()),
                             y + scale * static_cast<float>(character.pos.bottom()));
                }
                x_pos += static_cast<int>(scale * static_cast<float>(character.advance));
              }
              else if (is_large)
              {
                float scale = 2.0f;
                if (draw_it)
                {
                  va.color(top_color);
                  va.texcoord(character.uv.left(), character.uv.top());
                  va.vertex(x + scale * static_cast<float>(character.pos.left()),
                             y + static_cast<float>(character.pos.top()));

                  va.color(top_color);
                  va.texcoord(character.uv.right(), character.uv.top());
                  va.vertex(x + scale * static_cast<float>(character.pos.right()),
                             y + static_cast<float>(character.pos.top()));

                  va.color(bottom_color);
                  va.texcoord(character.uv.right(), character.uv.bottom());
                  va.vertex(x + scale * static_cast<float>(character.pos.right()),
                             y + static_cast<float>(character.pos.bottom()));

                  va.color(bottom_color);
                  va.texcoord(character.uv.left(), character.uv.bottom());
                  va.vertex(x + scale * static_cast<float>(character.pos.left()),
                             y + static_cast<float>(character.pos.bottom()));
                }
                x_pos += static_cast<int>(scale * static_cast<float>(character.advance));
              }
              else
              {
                if (draw_it)
                {
                  va.color(top_color);
                  va.texcoord(character.uv.left(), character.uv.top());
                  va.vertex(x + static_cast<float>(character.pos.left()),
                             y + static_cast<float>(character.pos.top()));

                  va.color(top_color);
                  va.texcoord(character.uv.right(), character.uv.top());
                  va.vertex(x + static_cast<float>(character.pos.right()),
                             y + static_cast<float>(character.pos.top()));

                  va.color(bottom_color);
                  va.texcoord(character.uv.right(), character.uv.bottom());
                  va.vertex(x + static_cast<float>(character.pos.right()),
                             y + static_cast<float>(character.pos.bottom()));

                  va.color(bottom_color);
                  va.texcoord(character.uv.left(), character.uv.bottom());
                  va.vertex(x + static_cast<float>(character.pos.left()),
                             y + static_cast<float>(character.pos.bottom()));
                }
                x_pos += character.advance;
              }
            }
          }
        }
        break;
    }
  }

  if (impl->max_scroll_offset == -1.0f)
  {
    impl->max_scroll_offset = static_cast<float>(y_pos) - impl->rect.height();
    if (impl->max_scroll_offset < 0)
      impl->max_scroll_offset = 0;
  }

  // FIXME: This is not correct, since the last command itself might
  // not be completly excecuted
  if (i == impl->commands.end())
    impl->progress_complete = true;

  va.render(gc);
  gc.pop_matrix();

  impl->cursor_pos = glm::vec2(static_cast<float>(x_pos) + impl->rect.left(),
                              static_cast<float>(y_pos) + impl->rect.top());
}

void
TextArea::update(float delta)
{
  impl->passed_time += delta;
}

geom::frect
TextArea::get_rect() const
{
  return impl->rect;
}

glm::vec2
TextArea::get_cursor_pos() const
{
  return impl->cursor_pos;
}

float
TextArea::get_scroll_offset() const
{
  return impl->scroll_offset;
}

void
TextArea::set_scroll_offset(float s)
{
  if (s < 0.0f)
    impl->scroll_offset = 0.0f;
  else if (s > impl->max_scroll_offset)
    impl->scroll_offset = impl->max_scroll_offset;
  else
    impl->scroll_offset = s;
}

/* EOF */
