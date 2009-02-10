/*  $Id$
**   __      __ __             ___        __   __ __   __
**  /  \    /  \__| ____    __| _/_______/  |_|__|  | |  |   ____
**  \   \/\/   /  |/    \  / __ |/  ___/\   __\  |  | |  | _/ __ \
**   \        /|  |   |  \/ /_/ |\___ \  |  | |  |  |_|  |_\  ___/
**    \__/\  / |__|___|  /\____ /____  > |__| |__|____/____/\___  >
**         \/          \/      \/    \/                         \/
**  Copyright (C) 2005 Ingo Ruhnke <grumbel@gmx.de>
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

#ifndef HEADER_FRAMEBUFFER_HPP
#define HEADER_FRAMEBUFFER_HPP

#include "texture.hpp"

class FramebufferImpl;

/** */
class Framebuffer
{
public:
  Framebuffer();
  Framebuffer(GLenum target, int width, int height);
  ~Framebuffer();
  
  int get_width()  const;
  int get_height() const;
  Texture get_texture();

  GLuint get_handle() const;

  /** 
   * true if the Framebuffer is valid and usable, false if not 
   */
  operator bool() const;

  bool operator==(const Framebuffer&) const;
  bool operator!=(const Framebuffer&) const;

private:
  boost::shared_ptr<FramebufferImpl> impl;
};

#endif

/* EOF */
