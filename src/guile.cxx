//  $Id: guile.cxx,v 1.18 2003/08/20 00:15:10 grumbel Exp $
//
//  Feuerkraft - A Tank Battle Game
//  Copyright (C) 2000 Ingo Ruhnke <grumbel@gmx.de>
//
//  This program is free software; you can redistribute it and/or
//  modify it under the terms of the GNU General Public License
//  as published by the Free Software Foundation; either version 2
//  of the License, or (at your option) any later version.
//
//  This program is distributed in the hope that it will be useful,
//  but WITHOUT ANY WARRANTY; without even the implied warranty of
//  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
//  GNU General Public License for more details.
//
//  You should have received a copy of the GNU General Public License
//  along with this program; if not, write to the Free Software
//  Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.

#include <assert.h>
#include <libguile.h>
#include <ClanLib/Core/System/cl_assert.h>

#include "guile.hxx"

namespace Guile {
std::string
scm2string (SCM data)
{
  std::string str;
  
  if (scm_string_p(data))
    {
      char* tmpstr = scm_to_utf8_string(data);
      str = tmpstr;
      free(tmpstr);
    } else {
      SCM scmstr = scm_make_string(scm_from_int(0), SCM_UNDEFINED);
      SCM port = scm_mkstrport(SCM_INUM0, scmstr,
                               SCM_OPN | SCM_WRTNG, "scm_mkstrport");
      scm_display(data, port);
      char* tmpstr = scm_to_utf8_string(scmstr);
      str = tmpstr;
      free(tmpstr);
    }
  return str;
}

SCM pos2scm (int x, int y)
{
  return SCM_BOOL_F;/*scm_listify (scm_from_utf8_symbol ("pos"),
                      scm_from_int (x),
                      scm_from_int (y), 
                      SCM_UNDEFINED);*/
}

void pretty_print (std::ostream& stream, SCM obj)
{
  std::cout << "pretty_print" << std::endl;
  // FIXME: ...lalala..
  scm_write(obj, SCM_UNDEFINED);
  scm_newline(SCM_UNDEFINED);

}

bool equal_p(SCM a, SCM b)
{
  return SCM_NFALSEP(scm_equal_p(a, b));
}

SCM symbol2scm(const char* str)
{
  return scm_from_utf8_symbol(str);
}

std::string keyword2string(SCM keyword)
{
  assert(scm_keyword_p(keyword));
  //puts("keyword2string: ");
  //scm_display(keyword, SCM_UNDEFINED);
  //scm_newline(SCM_UNDEFINED);
  //scm_display(scm_keyword_dash_symbol(keyword), SCM_UNDEFINED);
  //scm_newline(SCM_UNDEFINED);

  char* str = scm_to_utf8_string(keyword);
  std::string ret = str + 1; // skip the dash
  free(str);
  return ret;
}

std::string symbol2string(SCM symbol)
{
  char* c_str = scm_to_utf8_string(symbol);
  std::string str = c_str;
  free(c_str);
  return str;
}

void enter_repl()
{
  SCM func = scm_c_lookup("feuerkraft:repl");
  if (func != SCM_BOOL_F)
    {
      scm_call_0(func);
    }
  else
    {
      std::cout << "### Error: feuerkraft.scm must be loaded to use the repl!" << std::endl;
    }
}

void enable_debug()
{
#if 0 // OLD Guile
  SCM_DEVAL_P = 1;
  SCM_BACKTRACE_P = 1;
  SCM_RECORD_POSITIONS_P = 1;
  SCM_RESET_DEBUG_MODE;
#else
  scm_c_eval_string("(debug-enable 'debug)"
                 "(debug-enable 'backtrace)"
                 "(read-enable  'positions)");
#endif
}

/** Disable all debugging */
void disable_debug()
{
  scm_c_eval_string("(debug-disable 'debug)"
                 "(debug-disable 'backtrace)"
                 "(read-disable  'positions)");
}

void enable_readline()
{
#ifdef WITH_STATIC_READLINE
  std::cout << "Loading readline... " << std::endl;
  scm_init_readline();
#endif

  scm_c_eval_string("(use-modules (ice-9 readline))"
                 "(activate-readline)");
}

} // namespace Guile

/* EOF */
