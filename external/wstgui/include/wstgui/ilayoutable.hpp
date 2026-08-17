/*
**  Windstille - A Sci-Fi Action-Adventure Game
**  Copyright (C) 2018 Ingo Ruhnke <grumbel@gmail.com>
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

#ifndef HEADER_WSTGUI_LAYOUTABLE_HPP
#define HEADER_WSTGUI_LAYOUTABLE_HPP

#include <optional>

#include <geom/rect.hpp>

namespace wstgui {

class Component;

class ILayoutable
{
public:
  virtual ~ILayoutable() {}

  virtual void set_geometry(geom::frect const& rect) {}
  virtual std::optional<geom::fsize> get_size_hint() const { return std::nullopt; }
};

class SpacerLayoutable : public ILayoutable
{
public:
  SpacerLayoutable(geom::fsize const& size_hint) :
    m_size_hint(size_hint)
  {}

  std::optional<geom::fsize> get_size_hint() const override { return m_size_hint; }

private:
  geom::fsize m_size_hint;
};

class LayoutableComponent : public ILayoutable
{
public:
  LayoutableComponent(Component* component);
  virtual ~LayoutableComponent() {}

  void set_geometry(geom::frect const& rect) override;

private:
  Component* m_component;

public:
  LayoutableComponent(const LayoutableComponent&) = delete;
  LayoutableComponent& operator=(const LayoutableComponent&) = delete;
};

} // namespace wstgui

#endif

/* EOF */
