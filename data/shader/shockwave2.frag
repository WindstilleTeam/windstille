// -*- c++ -*-

// Shockwave shader, good look version

uniform sampler2DRect background_tex;
uniform sampler2D noise_tex;
uniform float     radius;

void main()
{
  float thickness = 164.0;

  float x_dist = gl_TexCoord[0].x - 0.5;
  float y_dist = gl_TexCoord[0].y - 0.5;
  // Distance from center of the explosion
  float dist   = sqrt(x_dist*x_dist + y_dist*y_dist);

  vec4 uv_col  = texture2D(noise_tex, gl_TexCoord[0].xy*1.5);
  float h = 1.0 - clamp((radius - dist)*512.0/thickness, 0.0, 1.0);
  h *= h;
  vec4 col = texture2DRect(background_tex, gl_FragCoord.xy + (uv_col.rb * uv_col.g) * h * 0.3 * 300.0);

  float highlight = 1.0 - clamp((radius - dist)*512.0/32.0, 0.0, 1.0);

  col += vec4(h * uv_col.r + highlight*0.9,
              h * uv_col.r + highlight*0.9,
              h*1.5 * uv_col.r + highlight, 
              1);

  col.a = clamp((250.0 - (radius*256.0))/50.0, 0.0, 1.0);

  gl_FragColor = col;
}

/* EOF */
