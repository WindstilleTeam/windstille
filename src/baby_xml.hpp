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

#ifndef HEADER_WINDSTILLE_BABY_XML_HXX
#define HEADER_WINDSTILLE_BABY_XML_HXX

#include <string>
#include <vector>

/** Parser for a flat XML-like tag soup, it doesn't support recursive
    structures */
class BabyXML
{
public:
  struct Attribute
  {
    std::string name;
    std::string value;
  };

  struct Node
  {
    enum Type { NONE, TEXT, START_TAG, END_TAG, ENTITY };
    typedef std::vector<Attribute> Attributes;

    Type type;
    std::string content;
    Attributes attributes;

    Node() : type(NONE) {}
    Node(Type type_, const std::string& content_ = std::string()) : type(type_), content(content_) {}
  };

  typedef std::vector<Node> Nodes;
  typedef Nodes::iterator       iterator;
  typedef Nodes::const_iterator const_iterator;

  Nodes nodes;

  /** Parse the given \a text and construct the nodes from it */
  BabyXML(const std::string& text);
  ~BabyXML();

  void add_node(const Node& node);

  iterator begin() { return nodes.begin(); }
  iterator end()   { return nodes.end(); }
};

#endif

/* EOF */
