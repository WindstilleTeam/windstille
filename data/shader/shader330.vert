#version 330 core

in vec2 texcoord;
in vec4 diffuse;
in vec3 position;

out vec2 texcoord_v;
out vec4 diffuse_v;

uniform mat4 modelviewprojection;

void main()
{
  texcoord_v = texcoord;
  diffuse_v = diffuse;
  gl_Position = modelviewprojection * vec4(position, 1.0);
}

/* EOF */
