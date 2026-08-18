/*
**  Windstille - A Sci-Fi Action-Adventure Game
**  Copyright (C) 2026 Ingo Ruhnke <grumbel@gmail.com>
**
**  This program is free software: you can redistribute it and/or modify
**  it under the terms of the GNU General Public License as published by
**  the Free Software Foundation, either version 3 of the License, or
**  (at your option) any later version.
*/

#ifndef HEADER_WINDSTILLE_HUD_VIRTUAL_GAMEPAD_HPP
#define HEADER_WINDSTILLE_HUD_VIRTUAL_GAMEPAD_HPP

#include <array>
#include <cstdint>
#include <string>
#include <vector>

#include <SDL.h>
#include <geom/geom.hpp>
#include <wstgui/screen.hpp>
#include <wstinput/controller.hpp>

#include "util/currenton.hpp"

namespace windstille {

/**
 * On-screen Xbox 360–style virtual gamepad for touch (Android) and mouse
 * testing. Rectangular hit regions for:
 *   - dual analog sticks (+ click)
 *   - D-pad
 *   - A/B/X/Y face buttons
 *   - LB/RB shoulders, LT/RT triggers
 *   - Back / Start / Guide
 *
 * Injects into the live Controller each frame via InputManagerSDL.
 */
class VirtualGamepad :
  public wstgui::Screen,
  public Currenton<VirtualGamepad>
{
public:
  VirtualGamepad();
  ~VirtualGamepad() override = default;

  void draw(wstdisplay::GraphicsContext& gc) override;
  void update(float delta, wstinput::Controller const& controller) override;
  void handle_event(const SDL_Event& event) override;

  void set_enabled(bool enabled) { m_enabled = enabled; }
  bool is_enabled() const { return m_enabled; }

  /** Rebuild layout for the current window size (call on resize). */
  void layout(int screen_w, int screen_h);

private:
  enum class Control : int {
    LeftStick = 0,
    RightStick,
    DpadUp,
    DpadDown,
    DpadLeft,
    DpadRight,
    FaceA,
    FaceB,
    FaceX,
    FaceY,
    LB,
    RB,
    LT,
    RT,
    Back,
    Start,
    Guide,
    LeftStickClick,
    RightStickClick,
    Count
  };

  struct Region {
    Control control;
    geom::frect rect;   // screen pixels
    std::string label;
  };

  struct FingerState {
    SDL_FingerID id = 0;
    bool active = false;
    Control control = Control::Count;
    float x = 0.f;
    float y = 0.f;
  };

  void ensure_layout(wstdisplay::GraphicsContext& gc);
  Region const* hit_test(float x, float y) const;
  void on_down(float x, float y, int64_t finger_id);
  void on_move(float x, float y, int64_t finger_id);
  void on_up(int64_t finger_id);
  void release_control(Control c);
  void inject_into_controller();
  void draw_region(wstdisplay::GraphicsContext& gc, Region const& r, bool pressed) const;

  bool m_enabled = true;
  int m_layout_w = 0;
  int m_layout_h = 0;
  std::vector<Region> m_regions;

  // Per-control pressed / axis state
  std::array<bool, static_cast<size_t>(Control::Count)> m_pressed{};
  float m_left_stick_x = 0.f;
  float m_left_stick_y = 0.f;
  float m_right_stick_x = 0.f;
  float m_right_stick_y = 0.f;
  float m_lt = 0.f;
  float m_rt = 0.f;

  // Stick base centers (pixel) for relative deflection
  float m_left_stick_cx = 0.f;
  float m_left_stick_cy = 0.f;
  float m_right_stick_cx = 0.f;
  float m_right_stick_cy = 0.f;
  float m_stick_radius = 1.f;

  static constexpr size_t kMaxFingers = 10;
  std::array<FingerState, kMaxFingers> m_fingers{};
};

} // namespace windstille

#endif

/* EOF */
