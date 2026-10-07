/*
**  Windstille - A Sci-Fi Action-Adventure Game
**  Copyright (C) 2026 Ingo Ruhnke <grumbel@gmail.com>
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

#ifndef HEADER_WINDSTILLE_EDITOR_OBSERVABLE_SIGNAL_HPP
#define HEADER_WINDSTILLE_EDITOR_OBSERVABLE_SIGNAL_HPP

#include <functional>
#include <vector>

namespace windstille {

template<typename Signature>
class ObservableSignal;

template<typename... Args>
class ObservableSignal<void(Args...)>
{
public:
  using Slot = std::function<void(Args...)>;

  void connect(Slot slot) { m_slots.push_back(std::move(slot)); }

  void operator()(Args... args) const
  {
    for (auto const& slot : m_slots) {
      if (slot) slot(args...);
    }
  }

  void clear() { m_slots.clear(); }

private:
  std::vector<Slot> m_slots;
};

} // namespace windstille

#endif

/* EOF */
