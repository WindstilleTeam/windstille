/*
**  Windstille - A Sci-Fi Action-Adventure Game
**  Copyright (C) 2005 Ingo Ruhnke <grumbel@gmx.de>
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

#include <assert.h>
#include "math/rect.hpp"
#include "font/fonts.hpp"
#include "screen/game_session.hpp"
#include "input/input_manager.hpp"
#include "engine/script_manager.hpp"
#include "display/display.hpp"
#include "scripting/util.hpp"
#include "console.hpp"

Console console;

class ConsoleStreambuf : public std::streambuf
{
public:
  ConsoleStreambuf(Console* console_)
    : console(console_)
  {
    setp(buf, buf+sizeof(buf));
  }

  ~ConsoleStreambuf()
  {
    sync();
  }

protected:
  virtual int overflow(int c)
  {
    if(pbase() == pptr())
      return 0;

    size_t size = pptr() - pbase();
    console->add(pbase(), size);
      
    if(c != traits_type::eof()) {
      char str[1];
      str[0] = c;
      console->add(str, 1);
    }

    setp(buf, buf + size);

    return 0;
  }

  virtual int sync()
  {
    return overflow(traits_type::eof());
  }

private:
  Console* console;
  char buf[1024];
};

//-------------------------------------------------------------------------------

class ConsoleImpl
{
public:
  Console& console;

  struct ConsoleEntry {
    std::string message;
    float display_time;
  };

  int x_pos;
  int y_pos;
  typedef std::vector<ConsoleEntry> Buffer;
  Buffer buffer;

  std::string command_line;
  bool active;
  std::vector<std::string> history;
  int history_position;

  int cursor_pos;

  int scroll_offset;
  ConsoleEntry current_entry;

  ConsoleImpl(Console& console_)
    : console(console_)
  {
    x_pos = 16;
    y_pos = 600-16;

    active = false;
    history_position = 1; 

    scroll_offset = 0;
    cursor_pos  = 0;   
  }

  void draw();
  void update(float delta);
  void eval_command_line();
  void tab_complete();

  /** adds a newline if the current line contains content */
  void maybe_newline();

  std::vector<std::string> get_roottable();

  void execute(const std::string& str);

  void add(char* buf, int len)
  {
    current_entry.display_time = 0;

    for (int i = 0; i < len; ++i)
      {
        if (buf[i] == '\n' || (current_entry.message.size() > 80 && buf[i] == ' ')) // primitive linebreak
          {
            buffer.push_back(current_entry);
            current_entry = ConsoleEntry();
          }
        else
          {
            current_entry.message += buf[i];
          }
      }
  }
};

//-------------------------------------------------------------------------------

void
ConsoleImpl::draw()
{
  int y = y_pos;

  if (active)
    y -= Fonts::ttffont->get_height() + 2;

  int num_lines = 600 / (Fonts::ttffont->get_height() + 2);

  if (console.is_active())
    Display::fill_rect(Rect(0,0, Display::get_width(), 600),
                       Color(0, 0, 0, 0.5f));

  for(int i = buffer.size()-1 - scroll_offset; i >= 0 && i > int(buffer.size()) - num_lines - scroll_offset; --i)
    {
      if (buffer[i].display_time < 5.0f || console.is_active())
        {
          float alpha = 1.0f;
          if (buffer[i].display_time > 4.0f && !console.is_active())
            alpha = 1.0f - (buffer[i].display_time - 4.0f);

          Fonts::ttffont->draw(Vector2f(x_pos, y), buffer[i].message, Color(0.88f, 0.88f, 1.0f, alpha));
        }
      y -= Fonts::ttffont->get_height() + 2;
    }

  if (active)
    {
      std::string str = command_line;
      if (SDL_GetTicks() % 300 > 150)
        {
          if (cursor_pos < int(str.size()))
            str[cursor_pos] = '_';
          else
            str += "_";
        }

      Fonts::ttffont->draw(Vector2f(x_pos, y_pos), "> " + str, Color(1.0f, 1.0f, 1.0f));
    }
}

void
ConsoleImpl::update(float delta)
{
  for(Buffer::iterator i = buffer.begin(); i != buffer.end(); ++i)
    {
      i->display_time += delta;
    }  

  if (active)
    {
      InputEventLst events = InputManager::get_controller().get_events();
  
      for (InputEventLst::iterator i = events.begin(); i != events.end(); ++i)
        {
          if ((*i).type == KEYBOARD_EVENT)
            {
              if ((*i).keyboard.key_type == KeyboardEvent::LETTER)
                {
                  if (cursor_pos == int(command_line.size()))
                    {
                      command_line += (char)(*i).keyboard.code;
                      cursor_pos += 1;
                    }
                  else
                    {
                      command_line.insert(cursor_pos, std::string(1, (char)(*i).keyboard.code));
                      cursor_pos += 1;
                    }
                }
              else if ((*i).keyboard.key_type == KeyboardEvent::SPECIAL)
                {
                  //console << "special: " << i->keyboard.code << std::endl;

                  switch (i->keyboard.code)
                    {
                    case SDLK_BACKSPACE:
                      if (!command_line.empty() && cursor_pos > 0)
                        {
                          command_line.erase(cursor_pos - 1, 1);
                          cursor_pos -= 1;
                        }
                      break;

                    case SDLK_DELETE:
                      if (!command_line.empty())
                        {
                          command_line.erase(cursor_pos, 1);
                        }
                      break;

                    case SDLK_DOWN:
                      if (!history.empty())
                        {
                          history_position += 1;
                          if (history_position > int(history.size())-1)
                            history_position = int(history.size())-1;
                          command_line = history[history_position];
                          cursor_pos = command_line.size();
                        }
                      break;

                    case SDLK_HOME:
                      cursor_pos = 0;
                      break;
                      
                    case SDLK_END:
                      cursor_pos = command_line.size();
                      break;
                        
                    case SDLK_PAGEUP:
                      console.scroll(10);
                      break;

                    case SDLK_PAGEDOWN:
                      console.scroll(-10);
                      break;

                    case SDLK_TAB:
                      tab_complete();
                      break;

                    case SDLK_UP:
                      if (!history.empty())
                        {
                          history_position -= 1;
                          if (history_position < 0)
                            history_position = 0;

                          command_line = history[history_position];
                          cursor_pos = command_line.size();
                        }
                      break;

                    case SDLK_LEFT:
                      cursor_pos -= 1;
                      if (cursor_pos < 0)
                        cursor_pos = 0;
                      break;

                    case SDLK_RIGHT:
                      cursor_pos += 1;
                      if (cursor_pos > int(command_line.size()))
                        cursor_pos = command_line.size();
                      break;

                    case SDLK_RETURN:
                      eval_command_line();
                      break;

                    case SDLK_ESCAPE:
                    case SDLK_F1:
                      console.deactive();
                      break;
                    }
                }
            }
        }
    }
}

void
ConsoleImpl::maybe_newline()
{
  console << std::flush;
  if (!current_entry.message.empty())
    {
      console << std::endl;
    }
}

std::vector<std::string>
ConsoleImpl::get_roottable()
{
  std::vector<std::string> roottable;
  HSQUIRRELVM v = ScriptManager::current()->get_vm();

  sq_pushroottable(v);

  //push your table/array here
  sq_pushnull(v);  //null iterator
  while(SQ_SUCCEEDED(sq_next(v,-2)))
    {
      // here -1 is the value and -2 is the key
      const SQChar *s;
      if (SQ_SUCCEEDED(sq_getstring(v,-2, &s)))
        {
          roottable.push_back((char*)s); // FIXME: likely not going to work on 64bit
        }
      else
        {
          console << "Unknown key type for element" << std::endl;
        }
                              
      sq_pop(v,2); //pops key and val before the nex iteration
    }
                          
  sq_pop(v, 1);
  
  return roottable;
}

static bool has_prefix(const std::string& lhs, const std::string rhs)
{
  if (lhs.length() < rhs.length())
    return false;
  else
    return lhs.compare(0, rhs.length(), rhs) == 0;
}

static std::string longest_prefix(const std::string& lhs, const std::string rhs)
{
  std::string::size_type i;
  for(i = 0; i < lhs.size() && i < rhs.size() && lhs[i] == rhs[i]; ++i);
  return lhs.substr(0, i);
}

static std::string find_longest_prefix(const std::vector<std::string>& lst)
{
  if (lst.empty())
    {
      return "";
    }
  else
    {
      std::string prefix = lst.front();

      for(std::vector<std::string>::const_iterator i = lst.begin() + 1; 
          i != lst.end(); ++i)
        {
          prefix = longest_prefix(prefix, *i);
        }

      return prefix;
    }
}

void
ConsoleImpl::tab_complete()
{
  const std::vector<std::string>& roottable = get_roottable();
  std::vector<std::string> completions;

  for(std::vector<std::string>::const_iterator i = roottable.begin();
      i != roottable.end();
      ++i)
    {
      if (has_prefix(*i, command_line))
        {
          completions.push_back(*i);
        }
    }

  if (completions.empty())
    {
      // console << "No completions" << std::endl;
    }
  else if (completions.size() == 1)
    {
      command_line = completions.front();
      cursor_pos = command_line.size();
    }
  else 
    {
      console << "> " << command_line << std::endl;
      for(std::vector<std::string>::iterator i = completions.begin(); i != completions.end(); ++i)
        {
          console << *i << " ";
        }
      console << std::endl;

      command_line = find_longest_prefix(completions);
      cursor_pos = command_line.size();
    }
}

void
ConsoleImpl::eval_command_line()
{
  if (!command_line.empty() && (history.empty() || history.back() != command_line))
    {
      history.push_back(command_line);
      history_position = history.size();
    }
                      
  console << "> " << command_line << std::endl;

  if (command_line == "quit" || command_line == "exit")
    {
      console.deactive();
    }
  else if (command_line == "help")
    {
      console << "This is a script console, can enter stuff in here that will then be evaluated.\n"
              << "Type 'quit' to exit the console." << std::endl;
    }
  else if (command_line == "reset")
    {
      GameSession::current()->set_sector("levels/newformat2.wst");
    }
  else if (command_line == "show")
    {
      HSQUIRRELVM v = ScriptManager::current()->get_vm();

      int size = sq_getsize(v, -1);
      console << size << " elements on the root table" << std::endl;

      sq_pushroottable(v);

      //push your table/array here
      sq_pushnull(v);  //null iterator
      while(SQ_SUCCEEDED(sq_next(v,-2)))
        {
          //here -1 is the value and -2 is the key
          const SQChar *s;
          if (SQ_SUCCEEDED(sq_getstring(v,-2, &s)))
            {
              console << s << " -> " << Scripting::squirrel2string(v, -1) << std::endl;
            }
          else
            {
              console << "Unknown key type for element" << std::endl;
            }
                              
          sq_pop(v,2); //pops key and val before the nex iteration
        }
                          
      sq_pop(v, 1);
    }
  else
    {
      execute(command_line);
      maybe_newline();
    }
  command_line = "";
  cursor_pos = 0;
}

void
ConsoleImpl::execute(const std::string& str_)
{
  std::string str = str_; //"return (" + str_ + ")";

  int i = str.length();
  const char* buffer = str.c_str();

  HSQUIRRELVM vm = ScriptManager::current()->get_vm();

  // Remember old stack position
  int oldtop = sq_gettop(vm); 

  try 
    {
      if(SQ_SUCCEEDED(sq_compilebuffer(vm, buffer, i, _SC("interactive console"), SQTrue)))
        {
          sq_pushroottable(vm);
          if(SQ_SUCCEEDED(sq_call(vm, 1, 1/*retval*/, true))) 
            {
              // FIXME: This does only work when somebody is doing a 'return', i.e. almost never
              if (sq_gettype(vm, -1) != OT_NULL)
                console << Scripting::squirrel2string(vm, -1) << std::endl;
              // else
              //   console << "(null)" << std::endl;
            }
        }
    } 
  catch(std::exception& e) 
    {
      std::cerr << "Couldn't execute command '" << str_ << "': "
                << e.what() << "\n";
    }

  // Reset to old stack position
  sq_settop(vm, oldtop);
}

//-------------------------------------------------------------------------------

Console::Console()
  : std::ostream(new ConsoleStreambuf(this)),
    impl(new ConsoleImpl(*this))
{
}

Console::~Console()
{
}

void
Console::activate()
{
  // Get rid of all input events so that we don't double press
  InputManager::clear();
  impl->active = true;

  SDL_EnableKeyRepeat(SDL_DEFAULT_REPEAT_DELAY, SDL_DEFAULT_REPEAT_INTERVAL);
}

void
Console::deactive()
{
  impl->active = false;

  SDL_EnableKeyRepeat(0, SDL_DEFAULT_REPEAT_INTERVAL);
}

bool
Console::is_active() const
{
  return impl->active;
}

void
Console::draw()
{
  impl->draw();
}

void
Console::update(float delta)
{
  impl->update(delta);
}

void
Console::add(char* buf, int len)
{
  impl->add(buf, len);
}

void
Console::scroll(int lines)
{
  impl->scroll_offset += lines;
  
  if (impl->scroll_offset < 0)
    impl->scroll_offset = 0;
  else if (impl->scroll_offset >= int(impl->buffer.size()))
    impl->scroll_offset = impl->buffer.size()-1;
}

void
Console::execute(const std::string& str)
{
  impl->execute(str);
}

/* EOF */
