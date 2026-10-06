// u_texture0: image, u_texture1: displacement, u_texture2: color
uniform vec2 rand_offset;
uniform float damp;

uniform sampler2D u_texture0;
uniform sampler2D u_texture1;
uniform sampler2D u_texture2;

varying vec2 v_texcoord;

void main(void)
{
  vec2 rnd1 = texture2D(u_texture1, v_texcoord + rand_offset).rg - vec2(0.5, 0.5);
  vec2 rnd2 = texture2D(u_texture1, v_texcoord + rand_offset + vec2(0.0, 0.1)).rg - vec2(0.5, 0.5);
  vec2 rnd3 = texture2D(u_texture1, v_texcoord + rand_offset + vec2(0.0, 0.2)).rg - vec2(0.5, 0.5);

  vec4 color = vec4(texture2D(u_texture0, v_texcoord + rnd1 * damp).r,
                    texture2D(u_texture0, v_texcoord + rnd2 * damp).g,
                    texture2D(u_texture0, v_texcoord + rnd3 * damp).b,
                    1.0);

  gl_FragColor = color;
}

/* EOF */
