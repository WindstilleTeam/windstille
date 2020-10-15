uniform vec2 rand_offset;
uniform float damp;

uniform sampler2D image_tex;
uniform sampler2D displace_tex;
uniform sampler2D color_tex;

vec4 contrast(vec4 color, float factor);
vec4 brightness(vec4 color, float factor);

void main(void)
{
  vec2 rnd1 = texture2D(displace_tex, gl_TexCoord[0].st + rand_offset).rg - vec2(0.5, 0.5);
  vec2 rnd2 = texture2D(displace_tex, gl_TexCoord[0].st + rand_offset + vec2(0.0, 0.1)).rg - vec2(0.5, 0.5);
  vec2 rnd3 = texture2D(displace_tex, gl_TexCoord[0].st + rand_offset + vec2(0.0, 0.2)).rg - vec2(0.5, 0.5);

  vec4 color = vec4(texture2D(image_tex, gl_TexCoord[0].st + rnd1 * damp).r,
                    texture2D(image_tex, gl_TexCoord[0].st + rnd2 * damp).g,
                    texture2D(image_tex, gl_TexCoord[0].st + rnd3 * damp).b,
                    1.0);

  gl_FragColor = color;
}

/* EOF */
