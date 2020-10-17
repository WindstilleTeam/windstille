#version 330 core

in vec2 texcoord;
in vec4 diffuse;
in vec3 position;

out vec2 texcoord_v;

uniform mat3 modelviewprojection;

void main()
{
  texcoord_v = texcoord;
  gl_Position = vec4(position * modelviewprojection, 1.0);
}

/* EOF */
