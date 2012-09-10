/*
**  Windstille - A Sci-Fi Action-Adventure Game
**  Copyright (C) 2005 Ingo Ruhnke <grumbel@gmx.de>
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

#include "objects/liquid.hpp"

#include "scenegraph/vertex_array_drawable.hpp"
#include "engine/sector.hpp"
#include "scenegraph/scene_graph.hpp"

#define SAMPLES 5

Liquid::Liquid(const FileReader& props) :
  texture(),
  t(),
  heightfield_store1(),
  heightfield_store2(),
  heightfield1(),
  heightfield2(),
  m_water_top(),
  m_water_body()
{
  int width = 10;
  t = 0;

  props.get("pos",    pos);
  props.get("width",  width);
  
  heightfield1 = &heightfield_store1;
  heightfield2 = &heightfield_store2;

  heightfield1->resize(width * SAMPLES, 0);
  heightfield2->resize(width * SAMPLES, 0);
  
  for(std::vector<float>::size_type i = 2; i < heightfield1->size()-2; ++i)
  {
    (*heightfield1)[i] = sinf(static_cast<float>(i) / static_cast<float>(heightfield1->size()) * 10.0f) * 0.5f
      + sinf(static_cast<float>(i) / static_cast<float>(heightfield1->size()) * 5.0f) * .5f;
    (*heightfield2)[i] = (*heightfield1)[i];
  }

  for(int i = 50; i < 70 && i < int(heightfield1->size()); ++i)
    (*heightfield1)[i] += 0.0025f;

  texture = Texture::create(Pathname("images/textures/water.png"));
  texture->set_wrap(GL_REPEAT);
  
  m_water_top.reset(new VertexArrayDrawable(Vector2f(pos.x, pos.y), 10000,
                                            Matrix(1.0f))); //sc.light().get_modelview()));

  m_water_body.reset(new VertexArrayDrawable(Vector2f(pos.x, pos.y), 10000,
                                             Matrix(1.0f))); // sc.light().get_modelview());

  Sector::current()->get_scene_graph().add_drawable(m_water_top);
  Sector::current()->get_scene_graph().add_drawable(m_water_body);
  update_scene_graph();
}

Liquid::~Liquid()
{
}

void
Liquid::update(float delta)
{
  { // Update the heightfield data
    t += delta * 1.0f;
    float factor = 0.1f * delta;

    for(int samples = 0; samples < 3; ++samples)
    {
      for(std::vector<float>::size_type i = 2; i < heightfield1->size()-2; ++i)
      {
        float value = 
          factor * ((*heightfield1)[i-2] +
                    (*heightfield1)[i-1] +
                    (*heightfield1)[i+1] +
                    (*heightfield1)[i+2])                      
          - (factor * 4 * (*heightfield1)[i])
          + (2*(*heightfield1)[i])
          - (*heightfield2)[i];
      
        (*heightfield2)[i] = value * 0.99999f;
      }
      std::swap(heightfield2, heightfield1);
    }
  }

  update_scene_graph();
}

void
Liquid::update_scene_graph()
{
  // Update the SceneGraph
  float texscale = 1.0f/128.0f;
  { // water top
    VertexArrayDrawable* array = m_water_top.get();
    array->clear();
    array->set_texture(texture);
    array->set_mode(GL_QUAD_STRIP);
    array->set_blend_func(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    for(std::vector<float>::size_type i = 0; i < heightfield1->size(); ++i)
    {
      float c = 0.5f;
      if (i > 0)
      {
        float angle = atan2f(32.0f* ((*heightfield1)[i] - (*heightfield1)[i-1]), 3.2f);
        c = std::min(1.0f, std::max(0.5f, 8.0f * (angle/float(M_PI)) + 0.5f));
      }

      array->color(Color(0.5f, 0.5f, 1.0f, 0.7f));
      array->texcoord((static_cast<float>(i) * 32.0f / static_cast<float>(SAMPLES)) * texscale + sinf(t + static_cast<float>(i)/10.0f)*0.2f,
                      (-32.0f * (*heightfield1)[i] + 8.0f) * texscale + sinf(t + static_cast<float>(i)/10.0f)*0.2f);
      array->vertex(static_cast<float>(i) * 32.0f / static_cast<float>(SAMPLES), -32.0f * (*heightfield1)[i] + 8.0f);

      array->color(Color(c, c, 1.0f, 1.0f));
      array->texcoord((static_cast<float>(i) * 32.0f / static_cast<float>(SAMPLES)) * texscale + sinf(t + static_cast<float>(i)/10.0f)*0.2f,
                      (-32.0f * (*heightfield1)[i]) * texscale + sinf(t + static_cast<float>(i)/10.0f)*0.2f);
      array->vertex(static_cast<float>(i) * 32.0f / static_cast<float>(SAMPLES), -32.0f * (*heightfield1)[i]);
    }
  }

  { // water body
    VertexArrayDrawable* array = m_water_body.get();
    array->clear();
    array->set_texture(texture);
    array->set_mode(GL_QUAD_STRIP);
    array->set_blend_func(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    for(std::vector<float>::size_type i = 0; i < heightfield1->size(); ++i)
    {
      array->color(Color(0.0f, 0.0f, 0.5f, 0.7f));
      array->texcoord((static_cast<float>(i) * 32.0f / static_cast<float>(SAMPLES)) * texscale + sinf(t + static_cast<float>(i)/10.0f)*0.2f,
                      (64.0f) * texscale + sinf(t+static_cast<float>(i)/10.0f)*0.2f);
      array->vertex(static_cast<float>(i) * 32.0f/static_cast<float>(SAMPLES), 64.0f);

      array->color(Color(0.5f, 0.5f, 1.0f, 0.7f));
      array->texcoord((static_cast<float>(i) * 32.0f / static_cast<float>(SAMPLES)) * texscale + sinf(t + static_cast<float>(i)/10.0f) * 0.2f,
                      (-32.0f * (*heightfield1)[i] + 8.0f) * texscale + sinf(t + static_cast<float>(i)/10.0f)*0.2f);
      array->vertex(static_cast<float>(i) * 32.0f/static_cast<float>(SAMPLES), -32.0f * (*heightfield1)[i] + 8.0f);
    }
  }
}

/* EOF */
