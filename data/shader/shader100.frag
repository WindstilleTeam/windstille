#version 100
precision mediump float;

uniform sampler2D diffuse_texture;

varying vec2 texcoord_v;
varying vec4 diffuse_v;

void main()
{
  gl_FragColor = texture2D(diffuse_texture, texcoord_v) * diffuse_v;
}

/* EOF */
