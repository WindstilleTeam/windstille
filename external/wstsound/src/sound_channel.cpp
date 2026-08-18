/*
**  Windstille - A Sci-Fi Action-Adventure Game
**  Copyright (C) 2010 Ingo Ruhnke <grumbel@gmail.com>
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

#include "sound_channel.hpp"

#include <filesystem>
#include <fstream>
#include <cstring>
#include <iostream>

#include "dummy_sound_source.hpp"
#include "sound_error.hpp"
#include "sound_file.hpp"
#include "sound_manager.hpp"
#include "sound_source.hpp"
#include "stream_sound_source.hpp"

namespace wstsound {

SoundChannel::SoundChannel(SoundManager& sound_manager) :
  m_sound_manager(sound_manager),
  m_sound_sources(),
  m_paused_sources(),
  m_gain(1.0f)
{
}

SoundSourcePtr
SoundChannel::play(std::filesystem::path const& filename,
                   SoundSourceType type)
{
  SoundSourcePtr source = prepare(filename, type);
  source->play();

  return source;
}

static bool sound_format_likely_supported(std::filesystem::path const& filename)
{
  std::ifstream in(filename, std::ios::binary);
  if (!in) {
    return false;
  }
  char magic[32] = {};
  in.read(magic, sizeof(magic));
  if (in.gcount() < 4) {
    return false;
  }
  if (std::strncmp(magic, "RIFF", 4) == 0) {
    return true; // WAV
  }
#if defined(WSTSOUND_WITH_MODPLUG)
  if (std::strncmp(magic, "IMPM", 4) == 0) {
    return true; // IT module
  }
#endif
#if defined(WSTSOUND_WITH_VORBIS)
  if (std::strncmp(magic, "OggS", 4) == 0) {
    return true;
  }
#endif
#if defined(WSTSOUND_WITH_OPUS)
  if (std::strncmp(magic, "OggS", 4) == 0) {
    return true;
  }
#endif
#if defined(WSTSOUND_WITH_MPG123)
  if ((static_cast<unsigned char>(magic[0]) == 0xff) ||
      (magic[0] == 'I' && magic[1] == 'D' && magic[2] == '3')) {
    return true;
  }
#endif
  return false;
}

SoundSourcePtr
SoundChannel::prepare(std::filesystem::path const& filename,
                      SoundSourceType type)
{
  // Skip formats we were not built with (e.g. .ogg without vorbis) without
  // throwing — deep-stack throw/catch is fragile on some hybrid toolchains.
  if (!sound_format_likely_supported(filename)) {
    std::cerr << "SoundChannel::prepare: unsupported or unreadable format: "
              << filename << std::endl;
    auto source = std::make_shared<DummySoundSource>();
    m_sound_sources.emplace_back(source);
    return source;
  }

  try
  {
    SoundSourcePtr source = m_sound_manager.create_sound_source(filename, *this, type);
    source->update_gain();

    m_sound_sources.emplace_back(source);
    return source;
  }
  catch(std::exception const& err)
  {
    std::cerr << "SoundChannel::prepare: Couldn't load " << filename << ": " << err.what() << std::endl;
    auto source = std::make_shared<DummySoundSource>();

    m_sound_sources.emplace_back(source);
    return source;
  }
  catch(...)
  {
    // Hybrid toolchains (e.g. R36S) can fail type matching for std::exception
    // across TUs; never let a sound load abort the process.
    std::cerr << "SoundChannel::prepare: Couldn't load " << filename << " (unknown error)\n";
    auto source = std::make_shared<DummySoundSource>();

    m_sound_sources.emplace_back(source);
    return source;
  }
}

SoundSourcePtr
SoundChannel::play(std::unique_ptr<SoundFile> sound_file,
                   SoundSourceType type)
{
  SoundSourcePtr source = prepare(std::move(sound_file), type);
  source->play();

  return source;
}

SoundSourcePtr
SoundChannel::prepare(std::unique_ptr<SoundFile> sound_file,
                      SoundSourceType type)
{
  SoundSourcePtr source = m_sound_manager.create_sound_source(std::move(sound_file), *this, type);
  source->update_gain();

  m_sound_sources.emplace_back(source);
  return source;
}

void
SoundChannel::set_gain(float gain)
{
  m_gain = gain;

  for(auto& source_wptr : m_sound_sources)
  {
    if (auto source = source_wptr.lock()) {
      source->update_gain();
    }
  }
}

float
SoundChannel::get_gain() const
{
  return m_gain;
}

void
SoundChannel::update(float delta)
{
  for(auto& source_wptr : m_sound_sources)
  {
    if (auto source = source_wptr.lock()) {
      source->update(delta);
    }
  }

  std::erase_if(m_sound_sources, [](SoundSourceWPtr const& source_wptr){
    return source_wptr.expired();
  });
}

void
SoundChannel::pause()
{
  for (auto& source_wptr : m_sound_sources) {
    if (auto source = source_wptr.lock()) {
      if (source->get_state() == SourceState::Playing) {
        source->pause();
        m_paused_sources.emplace_back(source);
      }
    }
  }
}

void
SoundChannel::resume()
{
  for (auto& source_wptr : m_paused_sources) {
    if (auto source = source_wptr.lock()) {
      source->play();
    }
  }

  m_paused_sources.clear();
}

void
SoundChannel::finish()
{
  for (auto& source_wptr : m_sound_sources) {
    if (auto source = source_wptr.lock()) {
      source->finish();
    }
  }
}

} // namespace wstsound

/* EOF */
