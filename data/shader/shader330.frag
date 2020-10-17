#version 330 core

uniform sampler2D diffuse_texture;

in vec2 texcoord_v;

layout(location = 0) out vec4 fragColor;

void main()
{
  fragColor = texture(diffuse_texture, texcoord_v);
}

/* EOF */
