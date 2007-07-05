/*  $Id$
**   __      __ __             ___        __   __ __   __
**  /  \    /  \__| ____    __| _/_______/  |_|__|  | |  |   ____
**  \   \/\/   /  |/    \  / __ |/  ___/\   __\  |  | |  | _/ __ \
**   \        /|  |   |  \/ /_/ |\___ \  |  | |  |  |_|  |_\  ___/
**    \__/\  / |__|___|  /\____ /____  > |__| |__|____/____/\___  >
**         \/          \/      \/    \/                         \/
**  Copyright (C) 2007 Ingo Ruhnke <grumbel@gmx.de>
**
**  This program is free software; you can redistribute it and/or
**  modify it under the terms of the GNU General Public License
**  as published by the Free Software Foundation; either version 2
**  of the License, or (at your option) any later version.
**
**  This program is distributed in the hope that it will be useful,
**  but WITHOUT ANY WARRANTY; without even the implied warranty of
**  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
**  GNU General Public License for more details.
** 
**  You should have received a copy of the GNU General Public License
**  along with this program; if not, write to the Free Software
**  Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA
**  02111-1307, USA.
*/

#ifndef HEADER_WIIMOTE_HPP
#define HEADER_WIIMOTE_HPP

#include "cwiid.h"

/** */
class Wiimote
{
public:
  static void error_callback(cwiid_wiimote_t*, const char *s, va_list ap);
  static void status_callback(cwiid_wiimote_t*, int mesg_count, union cwiid_mesg mesg[]);

private:
  cwiid_wiimote_t* m_wiimote;
  
public:
  Wiimote();
  
  void connect();
  void disconnect();

  // Callback functions
  void on_status  (const cwiid_status_mesg& msg);
  void on_error   (const cwiid_error_mesg& msg);
  void on_button  (const cwiid_btn_mesg& msg);
  void on_acc     (const cwiid_acc_mesg& msg);
  void on_ir      (const cwiid_ir_mesg& msg);
  void on_nunchuck(const cwiid_nunchuk_mesg& msg);
  void on_classic (const cwiid_classic_mesg& msg);
  void err(cwiid_wiimote_t*, const char *s, va_list ap);

private:
  Wiimote(const Wiimote&);
  Wiimote& operator=(const Wiimote&);
};

extern Wiimote* wiimote;

#endif

/* EOF */
