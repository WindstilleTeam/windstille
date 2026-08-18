/*
**  Windstille - A Sci-Fi Action-Adventure Game
**  Copyright (C) 2026 Ingo Ruhnke <grumbel@gmail.com>
**
**  This program is free software: you can redistribute it and/or modify
**  it under the terms of the GNU General Public License as published by
**  the Free Software Foundation, either version 3 of the License, or
**  (at your option) any later version.
*/

#include "hud/virtual_gamepad.hpp"

#include <algorithm>
#include <cmath>

#include <surf/color.hpp>
#include <wstdisplay/graphics_context.hpp>
#include <wstdisplay/opengl_window.hpp>
#include <wstinput/input_manager.hpp>

#include "app/app.hpp"
#include "app/controller_def.hpp"

namespace windstille {

namespace {

float clampf(float v, float lo, float hi)
{
  return std::max(lo, std::min(v, hi));
}

bool rect_contains(geom::frect const& r, float x, float y)
{
  return x >= r.left() && x < r.right() && y >= r.top() && y < r.bottom();
}

} // namespace

VirtualGamepad::VirtualGamepad()
{
  m_pressed.fill(false);
}

void
VirtualGamepad::layout(int screen_w, int screen_h)
{
  m_layout_w = screen_w;
  m_layout_h = screen_h;
  m_regions.clear();

  float const W = static_cast<float>(screen_w);
  float const H = static_cast<float>(screen_h);

  // Scale regions with the shorter side so phone/tablet layouts stay usable.
  float const s = std::min(W, H);

  auto add = [this](Control c, float x, float y, float w, float h, char const* label) {
    Region r;
    r.control = c;
    r.rect = geom::frect(x, y, x + w, y + h);
    r.label = label;
    m_regions.push_back(r);
  };

  // --- Left stick (bottom-left) ---
  float const stick = s * 0.28f;
  float const stick_pad = s * 0.03f;
  float const ls_x = stick_pad;
  float const ls_y = H - stick - stick_pad;
  add(Control::LeftStick, ls_x, ls_y, stick, stick, "LS");
  m_left_stick_cx = ls_x + stick * 0.5f;
  m_left_stick_cy = ls_y + stick * 0.5f;
  m_stick_radius = stick * 0.42f;

  // Left stick click: small pad on the stick itself (lower-right corner of LS)
  float const click = s * 0.07f;
  add(Control::LeftStickClick,
      ls_x + stick - click, ls_y + stick - click, click, click, "L3");

  // --- D-pad (above left stick) ---
  float const dpad = s * 0.09f;
  float const dpad_cx = ls_x + stick * 0.5f;
  float const dpad_cy = ls_y - dpad * 1.6f;
  add(Control::DpadUp,    dpad_cx - dpad * 0.5f, dpad_cy - dpad * 1.5f, dpad, dpad, "U");
  add(Control::DpadDown,  dpad_cx - dpad * 0.5f, dpad_cy + dpad * 0.5f, dpad, dpad, "D");
  add(Control::DpadLeft,  dpad_cx - dpad * 1.5f, dpad_cy - dpad * 0.5f, dpad, dpad, "L");
  add(Control::DpadRight, dpad_cx + dpad * 0.5f, dpad_cy - dpad * 0.5f, dpad, dpad, "R");

  // --- Right stick (bottom, left of face buttons) ---
  float const rs_x = W - stick * 2.15f - stick_pad;
  float const rs_y = H - stick - stick_pad;
  add(Control::RightStick, rs_x, rs_y, stick, stick, "RS");
  m_right_stick_cx = rs_x + stick * 0.5f;
  m_right_stick_cy = rs_y + stick * 0.5f;
  add(Control::RightStickClick,
      rs_x + stick - click, rs_y + stick - click, click, click, "R3");

  // --- Face buttons A/B/X/Y (right side, diamond) ---
  float const face = s * 0.095f;
  float const face_cx = W - face * 2.2f - stick_pad;
  float const face_cy = H - stick * 0.55f - stick_pad;
  add(Control::FaceA, face_cx - face * 0.5f, face_cy + face * 0.55f, face, face, "A");
  add(Control::FaceB, face_cx + face * 0.55f, face_cy - face * 0.5f, face, face, "B");
  add(Control::FaceX, face_cx - face * 1.55f, face_cy - face * 0.5f, face, face, "X");
  add(Control::FaceY, face_cx - face * 0.5f, face_cy - face * 1.55f, face, face, "Y");

  // --- Shoulders & triggers (top corners) ---
  float const sh_w = s * 0.18f;
  float const sh_h = s * 0.07f;
  float const tr_h = s * 0.07f;
  add(Control::LB, stick_pad, stick_pad + tr_h + s * 0.01f, sh_w, sh_h, "LB");
  add(Control::RB, W - stick_pad - sh_w, stick_pad + tr_h + s * 0.01f, sh_w, sh_h, "RB");
  add(Control::LT, stick_pad, stick_pad, sh_w, tr_h, "LT");
  add(Control::RT, W - stick_pad - sh_w, stick_pad, sh_w, tr_h, "RT");

  // --- Back / Guide / Start (top-center) ---
  float const mid_w = s * 0.12f;
  float const mid_h = s * 0.06f;
  float const mid_y = stick_pad + s * 0.02f;
  float const mid_gap = s * 0.02f;
  float const mid_total = mid_w * 3.f + mid_gap * 2.f;
  float const mid_x0 = (W - mid_total) * 0.5f;
  add(Control::Back,  mid_x0, mid_y, mid_w, mid_h, "Back");
  add(Control::Guide, mid_x0 + mid_w + mid_gap, mid_y, mid_w, mid_h, "Guide");
  add(Control::Start, mid_x0 + (mid_w + mid_gap) * 2.f, mid_y, mid_w, mid_h, "Start");
}

void
VirtualGamepad::ensure_layout(wstdisplay::GraphicsContext& gc)
{
  int const w = gc.size().width();
  int const h = gc.size().height();
  if (w != m_layout_w || h != m_layout_h || m_regions.empty()) {
    layout(w, h);
  }
}

VirtualGamepad::Region const*
VirtualGamepad::hit_test(float x, float y) const
{
  // Prefer smaller / later regions so stick-click wins over stick base.
  for (auto it = m_regions.rbegin(); it != m_regions.rend(); ++it) {
    if (rect_contains(it->rect, x, y)) {
      return &(*it);
    }
  }
  return nullptr;
}

void
VirtualGamepad::on_down(float x, float y, int64_t finger_id)
{
  Region const* hit = hit_test(x, y);
  if (!hit) {
    return;
  }

  FingerState* slot = nullptr;
  for (auto& f : m_fingers) {
    if (!f.active) {
      slot = &f;
      break;
    }
  }
  if (!slot) {
    return;
  }

  slot->active = true;
  slot->id = finger_id;
  slot->control = hit->control;
  slot->x = x;
  slot->y = y;

  m_pressed[static_cast<size_t>(hit->control)] = true;

  switch (hit->control) {
    case Control::LeftStick: {
      float dx = (x - m_left_stick_cx) / m_stick_radius;
      float dy = (y - m_left_stick_cy) / m_stick_radius;
      float len = std::sqrt(dx * dx + dy * dy);
      if (len > 1.f) {
        dx /= len;
        dy /= len;
      }
      m_left_stick_x = dx;
      m_left_stick_y = dy;
      break;
    }
    case Control::RightStick: {
      float dx = (x - m_right_stick_cx) / m_stick_radius;
      float dy = (y - m_right_stick_cy) / m_stick_radius;
      float len = std::sqrt(dx * dx + dy * dy);
      if (len > 1.f) {
        dx /= len;
        dy /= len;
      }
      m_right_stick_x = dx;
      m_right_stick_y = dy;
      break;
    }
    case Control::LT:
      m_lt = 1.f;
      break;
    case Control::RT:
      m_rt = 1.f;
      break;
    default:
      break;
  }
  inject_into_controller();
}

void
VirtualGamepad::on_move(float x, float y, int64_t finger_id)
{
  for (auto& f : m_fingers) {
    if (!f.active || f.id != finger_id) {
      continue;
    }
    f.x = x;
    f.y = y;
    if (f.control == Control::LeftStick) {
      float dx = (x - m_left_stick_cx) / m_stick_radius;
      float dy = (y - m_left_stick_cy) / m_stick_radius;
      float len = std::sqrt(dx * dx + dy * dy);
      if (len > 1.f) {
        dx /= len;
        dy /= len;
      }
      m_left_stick_x = dx;
      m_left_stick_y = dy;
    } else if (f.control == Control::RightStick) {
      float dx = (x - m_right_stick_cx) / m_stick_radius;
      float dy = (y - m_right_stick_cy) / m_stick_radius;
      float len = std::sqrt(dx * dx + dy * dy);
      if (len > 1.f) {
        dx /= len;
        dy /= len;
      }
      m_right_stick_x = dx;
      m_right_stick_y = dy;
    }
    inject_into_controller();
    return;
  }
}

void
VirtualGamepad::release_control(Control c)
{
  m_pressed[static_cast<size_t>(c)] = false;
  switch (c) {
    case Control::LeftStick:
      m_left_stick_x = 0.f;
      m_left_stick_y = 0.f;
      break;
    case Control::RightStick:
      m_right_stick_x = 0.f;
      m_right_stick_y = 0.f;
      break;
    case Control::LT:
      m_lt = 0.f;
      break;
    case Control::RT:
      m_rt = 0.f;
      break;
    default:
      break;
  }
}

void
VirtualGamepad::on_up(int64_t finger_id)
{
  for (auto& f : m_fingers) {
    if (!f.active || f.id != finger_id) {
      continue;
    }
    release_control(f.control);
    f.active = false;
    f.control = Control::Count;
    inject_into_controller();
    return;
  }
}

void
VirtualGamepad::inject_into_controller()
{
  auto& ctrl = g_app.input().get_controller_mut();

  auto set_btn = [&](Control c, int event) {
    bool const down = m_pressed[static_cast<size_t>(c)];
    bool const was = ctrl.get_button_state(event);
    if (down != was) {
      ctrl.add_button_event(event, down);
    } else {
      ctrl.set_button_state(event, down);
    }
  };

  set_btn(Control::FaceA, PRIMARY_BUTTON);
  set_btn(Control::FaceB, SECONDARY_BUTTON);
  set_btn(Control::FaceX, TERTIARY_BUTTON);
  set_btn(Control::FaceY, QUATERNARY_BUTTON);
  set_btn(Control::LB, LEFT_SHOULDER_BUTTON);
  set_btn(Control::RB, RIGHT_SHOULDER_BUTTON);
  set_btn(Control::Back, SELECT_BUTTON);
  set_btn(Control::Start, START_BUTTON);
  set_btn(Control::Guide, DEBUG_BUTTON);
  set_btn(Control::LeftStickClick, LEFT_STICK_BUTTON);
  set_btn(Control::RightStickClick, RIGHT_STICK_BUTTON);
  set_btn(Control::DpadUp, MENU_UP_BUTTON);
  set_btn(Control::DpadDown, MENU_DOWN_BUTTON);
  set_btn(Control::DpadLeft, MENU_LEFT_BUTTON);
  set_btn(Control::DpadRight, MENU_RIGHT_BUTTON);

  // Analog sticks (Y inverted so up is negative like a classic pad when needed —
  // Windstille uses screen-space: positive Y typically down; match joystick).
  ctrl.set_axis_state(X_AXIS, clampf(m_left_stick_x, -1.f, 1.f));
  ctrl.set_axis_state(Y_AXIS, clampf(m_left_stick_y, -1.f, 1.f));
  ctrl.set_axis_state(X2_AXIS, clampf(m_right_stick_x, -1.f, 1.f));
  ctrl.set_axis_state(Y2_AXIS, clampf(m_right_stick_y, -1.f, 1.f));

  // Triggers: store as axis in [-1,1] where 1 is fully pressed (matches
  // get_trigger_state which does axis/2+0.5).
  auto set_trigger = [&](int event, float amount) {
    float const axis = amount * 2.f - 1.f; // 0→-1, 1→+1
    ctrl.set_axis_state(event, axis);
  };
  set_trigger(LEFT_TRIGGER_AXIS, m_lt);
  set_trigger(RIGHT_TRIGGER_AXIS, m_rt);
}

void
VirtualGamepad::update(float /*delta*/, wstinput::Controller const& /*controller*/)
{
  if (!m_enabled) {
    return;
  }
  inject_into_controller();
}

void
VirtualGamepad::handle_event(const SDL_Event& event)
{
  if (!m_enabled) {
    return;
  }

  switch (event.type) {
    case SDL_FINGERDOWN: {
      float x = event.tfinger.x * static_cast<float>(m_layout_w > 0 ? m_layout_w : 1);
      float y = event.tfinger.y * static_cast<float>(m_layout_h > 0 ? m_layout_h : 1);
      // If layout not ready yet, use window size from SDL
      if (m_layout_w <= 0) {
        SDL_Window* win = SDL_GL_GetCurrentWindow();
        if (win) {
          int w = 0, h = 0;
          SDL_GetWindowSize(win, &w, &h);
          layout(w, h);
          x = event.tfinger.x * static_cast<float>(w);
          y = event.tfinger.y * static_cast<float>(h);
        }
      }
      on_down(x, y, event.tfinger.fingerId);
      break;
    }
    case SDL_FINGERMOTION: {
      float x = event.tfinger.x * static_cast<float>(std::max(1, m_layout_w));
      float y = event.tfinger.y * static_cast<float>(std::max(1, m_layout_h));
      on_move(x, y, event.tfinger.fingerId);
      break;
    }
    case SDL_FINGERUP:
      on_up(event.tfinger.fingerId);
      break;

    // Mouse fallback for desktop testing of the Android pad layout.
    case SDL_MOUSEBUTTONDOWN:
      if (event.button.button == SDL_BUTTON_LEFT) {
        on_down(static_cast<float>(event.button.x),
                static_cast<float>(event.button.y),
                -1);
      }
      break;
    case SDL_MOUSEMOTION:
      if (event.motion.state & SDL_BUTTON_LMASK) {
        on_move(static_cast<float>(event.motion.x),
                static_cast<float>(event.motion.y),
                -1);
      }
      break;
    case SDL_MOUSEBUTTONUP:
      if (event.button.button == SDL_BUTTON_LEFT) {
        on_up(-1);
      }
      break;

    default:
      break;
  }
}

void
VirtualGamepad::draw_region(wstdisplay::GraphicsContext& gc,
                            Region const& r, bool pressed) const
{
  surf::Color fill = pressed
    ? surf::Color(0.2f, 0.7f, 1.0f, 0.55f)
    : surf::Color(1.0f, 1.0f, 1.0f, 0.22f);
  surf::Color border = pressed
    ? surf::Color(1.0f, 1.0f, 1.0f, 0.9f)
    : surf::Color(1.0f, 1.0f, 1.0f, 0.45f);

  gc.fill_rounded_rect(r.rect, 6.0f, fill);
  gc.draw_rounded_rect(r.rect, 6.0f, border);
}

void
VirtualGamepad::draw(wstdisplay::GraphicsContext& gc)
{
  if (!m_enabled) {
    return;
  }

  ensure_layout(gc);

  for (Region const& r : m_regions) {
    bool pressed = m_pressed[static_cast<size_t>(r.control)];
    draw_region(gc, r, pressed);

    // Stick nub
    if (r.control == Control::LeftStick) {
      float nx = m_left_stick_cx + m_left_stick_x * m_stick_radius * 0.7f;
      float ny = m_left_stick_cy + m_left_stick_y * m_stick_radius * 0.7f;
      gc.fill_circle(geom::fpoint(nx, ny), m_stick_radius * 0.28f,
                     pressed ? surf::Color(0.2f, 0.7f, 1.0f, 0.85f)
                             : surf::Color(1.0f, 1.0f, 1.0f, 0.4f));
    } else if (r.control == Control::RightStick) {
      float nx = m_right_stick_cx + m_right_stick_x * m_stick_radius * 0.7f;
      float ny = m_right_stick_cy + m_right_stick_y * m_stick_radius * 0.7f;
      gc.fill_circle(geom::fpoint(nx, ny), m_stick_radius * 0.28f,
                     pressed ? surf::Color(0.2f, 0.7f, 1.0f, 0.85f)
                             : surf::Color(1.0f, 1.0f, 1.0f, 0.4f));
    }
  }
}

} // namespace windstille

/* EOF */
