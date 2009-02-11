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

#include "display/vertex_array_drawing_request.hpp"
#include "math/random.hpp"
#include "liquid.hpp"

#define SAMPLES 5

Liquid::Liquid(FileReader& props)
{
  int width = 10;
  t = 0;

  props.get("pos",    pos);
  props.get("width",  width);
  props.print_unused_warnings("Liquid");

  heightfield1 = &heightfield_store1;
  heightfield2 = &heightfield_store2;

  heightfield1->resize(width * SAMPLES, 0);
  heightfield2->resize(width * SAMPLES, 0);
  
  for(std::vector<float>::size_type i = 2; i < heightfield1->size()-2; ++i)
    {
      (*heightfield1)[i] = sin(i/float(heightfield1->size()) * 10.0f) * 0.5f
        + sin(i/float(heightfield1->size()) * 5.0f) * .5f;
      (*heightfield2)[i] = (*heightfield1)[i];
    }

  for(int i = 50; i < 70 && i < int(heightfield1->size()); ++i)
    (*heightfield1)[i] += 0.0025f;

  texture = Texture("images/textures/water.png");
  texture.set_wrap(GL_REPEAT);
}

Liquid::~Liquid()
{
}

void
Liquid::draw(SceneContext& sc)
{
  float texscale = 1.0f/128.0f;
  {
    VertexArrayDrawingRequest* array = new VertexArrayDrawingRequest(Vector(pos.x, pos.y), 10000,
                                                                     sc.light().get_modelview());
    array->set_texture(texture);
    array->set_mode(GL_QUAD_STRIP);
    array->set_blend_func(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    for(std::vector<float>::size_type i = 0; i < heightfield1->size(); ++i)
      {
        float c = 0.5f;
        if (i > 0)
          {
            float angle = atan2(32.0f* ((*heightfield1)[i] - (*heightfield1)[i-1]), 3.2f);
            c = std::min(1.0f, std::max(0.5f, 8.0f * (angle/float(M_PI)) + 0.5f));
          }

        array->color(Color(0.5f, 0.5f, 1.0f, 0.7f));
        array->texcoord((i * 32.0f/SAMPLES) * texscale + sin(t + i/10.0f)*0.2f,
                        (-32.0f * (*heightfield1)[i] + 8.0f) * texscale + sin(t+i/10.0f)*0.2f);
        array->vertex(i * 32.0f/SAMPLES, -32.0f * (*heightfield1)[i] + 8.0f);

        array->color(Color(c, c, 1.0f, 1.0f));
        array->texcoord((i * 32.0f/SAMPLES) * texscale + sin(t + i/10.0f)*0.2f,
                        (-32.0f * (*heightfield1)[i]) * texscale + sin(t+i/10.0f)*0.2f);
        array->vertex(i * 32.0f/SAMPLES, -32.0f * (*heightfield1)[i]);
      }

    sc.color().draw(array);
  }
  {
    VertexArrayDrawingRequest* array = new VertexArrayDrawingRequest(Vector(pos.x, pos.y), 10000,
                                                                     sc.light().get_modelview());
    array->set_texture(texture);
    array->set_mode(GL_QUAD_STRIP);
    array->set_blend_func(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    for(std::vector<float>::size_type i = 0; i < heightfield1->size(); ++i)
      {
        array->color(Color(0.0f, 0.0f, 0.5f, 0.7f));
        array->texcoord((i * 32.0f/SAMPLES) * texscale + sin(t + i/10.0f)*0.2f,
                        (64.0f) * texscale + sin(t+i/10.0f)*0.2f);
        array->vertex(i * 32.0f/SAMPLES, 64.0f);

        array->color(Color(0.5f, 0.5f, 1.0f, 0.7f));
        array->texcoord((i * 32.0f/SAMPLES) * texscale + sin(t + i/10.0f)*0.2f,
                        (-32.0f * (*heightfield1)[i] + 8.0f) * texscale + sin(t+i/10.0f)*0.2f);
        array->vertex(i * 32.0f/SAMPLES, -32.0f * (*heightfield1)[i] + 8.0f);
      }

    sc.color().draw(array);
  }
}

void
Liquid::update(float delta)
{
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

/* EOF */
