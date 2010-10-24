uniform vec2 offset;
uniform vec2 rand_offset;

uniform sampler2D image_tex;
uniform sampler2D displace_tex;
uniform sampler2D color_tex;

vec4 contrast(vec4 color, float factor);
vec4 brightness(vec4 color, float factor);

void main(void)
{
  vec4 rnd1 =  texture2D(displace_tex, gl_TexCoord[0].st + rand_offset + vec2(0.25,0.25));
  vec4 rnd2 =  texture2D(displace_tex, gl_TexCoord[0].st + rand_offset + vec2(0.125,0.125));
  vec4 rnd3 =  texture2D(displace_tex, gl_TexCoord[0].st + rand_offset + vec2(0.5,0.5));

  vec4 color = vec4(texture2D(image_tex, gl_TexCoord[0].st + offset + rnd1*0.03).r,
                    texture2D(image_tex, gl_TexCoord[0].st + offset + rnd2*0.03).g,
                    texture2D(image_tex, gl_TexCoord[0].st + offset + rnd3*0.03).b,
                    1.0);

  gl_FragColor = color;
}

/* EOF */
