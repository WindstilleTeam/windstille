#version 100

attribute vec2 texcoord;
attribute vec4 diffuse;
attribute vec3 position;

varying vec2 texcoord_v;
varying vec4 diffuse_v;

uniform mat4 modelviewprojection;

void main()
{
  texcoord_v = texcoord;
  diffuse_v = diffuse;
  gl_Position = modelviewprojection * vec4(position, 1.0);
}

/* EOF */
