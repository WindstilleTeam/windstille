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

#include <assert.h>
#include "wiimote.hpp"

Wiimote* wiimote = 0;

Wiimote::Wiimote()
  : m_wiimote(0)
{
  assert(wiimote == 0);
  wiimote = this;
  
  cwiid_set_err(&Wiimote::error_callback);
}

void
Wiimote::connect()
{
  assert(m_wiimote == 0);

  /* Connect to any wiimote */
  bdaddr_t bdaddr = *BDADDR_ANY;

  /* Connect to address in string WIIMOTE_BDADDR */
  /* str2ba(WIIMOTE_BDADDR, &bdaddr); */

  /* Connect to the wiimote */
  printf("Put Wiimote in discoverable mode now (press 1+2)...\n");

  if (!(m_wiimote = cwiid_connect(&bdaddr, 0))) { // FIXME: flags?
    fprintf(stderr, "Unable to connect to wiimote\n");
  } else {
    cwiid_set_mesg_callback(m_wiimote, &Wiimote::status_callback);
  }
}

void
Wiimote::disconnect()
{
  if (m_wiimote)
    {
      cwiid_disconnect(m_wiimote);
      m_wiimote = 0;
    }
}

void
Wiimote::on_status(const cwiid_status_mesg& msg)
{
  printf("Status Report: battery=%d extension=",
         msg.battery);
  switch (msg.ext_type)
    {

    case CWIID_EXT_NONE:
      printf("none");
      break;

    case CWIID_EXT_NUNCHUK:
      printf("Nunchuk");
      break;

    case CWIID_EXT_CLASSIC:
      printf("Classic Controller");
      break;
    default:
      printf("Unknown Extension");
      break;
    }
  printf("\n");
}

void
Wiimote::on_error(const cwiid_error_mesg& msg)
{
  if (m_wiimote)
    {
      if (cwiid_disconnect(m_wiimote)) {
        fprintf(stderr, "Error on wiimote disconnect\n");
        m_wiimote = 0;
      }
    }
}

void
Wiimote::on_button(const cwiid_btn_mesg& msg)
{
  printf("Button Report: %.4X\n", msg.buttons);
}

void
Wiimote::on_acc(const cwiid_acc_mesg& msg)
{
  printf("Acc Report: x=%d, y=%d, z=%d\n", msg.acc[0], msg.acc[1], msg.acc[2]);
}

void
Wiimote::on_ir(const cwiid_ir_mesg& msg)
{
  printf("IR Report: ");
  for (int i = 0; i < CWIID_IR_SRC_COUNT; ++i)
    {
      if (msg.src[i].valid) {
        printf("(%d,%d) ", msg.src[i].pos[0], msg.src[i].pos[1]);
      }
    }
}

void
Wiimote::on_nunchuck(const cwiid_nunchuk_mesg& msg)
{
  printf("Nunchuk Report: btns=%.2X stick=(%d,%d) acc.x=%d acc.y=%d acc.z=%d\n", 
         msg.buttons,
         msg.stick[0], msg.stick[1], 
         msg.acc[0], msg.acc[1], msg.acc[2]);
}

void
Wiimote::on_classic(const cwiid_classic_mesg& msg)
{
  printf("Classic Report: btns=%.4X l_stick=(%d,%d) r_stick=(%d,%d) "
         "l=%d r=%d\n", msg.buttons,
         msg.l_stick[0], msg.l_stick[1],
         msg.r_stick[0], msg.r_stick[1],
         msg.l, msg.r);
}

void
Wiimote::err(cwiid_wiimote_t* w, const char *s, va_list ap)
{
 if (w) 
    printf("%d:", cwiid_get_id(w));
  else 
    printf("-1:");

  vprintf(s, ap);
  printf("\n");  
}

void
Wiimote::error_callback(cwiid_wiimote_t* w, const char *s, va_list ap)
{
  wiimote->err(w, s, ap);
}

void
Wiimote::status_callback(cwiid_wiimote_t*, int mesg_count, union cwiid_mesg mesg[])
{
  for (int i=0; i < mesg_count; i++)
    {
      switch (mesg[i].type) 
        {
        case CWIID_MESG_STATUS:
          wiimote->on_status(mesg[i].status_mesg);
          break;

        case CWIID_MESG_BTN:
          wiimote->on_button(mesg[i].btn_mesg);
          break;

        case CWIID_MESG_ACC:
          wiimote->on_acc(mesg[i].acc_mesg);
          break;

        case CWIID_MESG_IR:
          wiimote->on_ir(mesg[i].ir_mesg);
          break;

        case CWIID_MESG_NUNCHUK:
          wiimote->on_nunchuck(mesg[i].nunchuk_mesg);
          break;

        case CWIID_MESG_CLASSIC:
          wiimote->on_classic(mesg[i].classic_mesg);
          break;

        case CWIID_MESG_ERROR:
          wiimote->on_error(mesg[i].error_mesg);
          break;

        default:
          printf("Wiimote: Unknown Report");
          break;
        }
    }
}

/* EOF */
