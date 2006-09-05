// -*- c++ -*-

uniform float radius;
uniform sampler2D background_tex;
uniform sampler2D noise_tex;

void main()
{
  if (0)
    {
      float deform_factor = 50.0;

      vec4 noise_color = texture2D(noise_tex, gl_TexCoord[0].xy * 2.0 + gl_FragCoord.xy * 0.005);

      vec4 noise_color2 = texture2D(noise_tex, gl_TexCoord[0].xy * 2.0 + vec2(0.5, 0.5));

      vec2 offset = ((noise_color.xy - vec2(0.5, 0.5)) * deform_factor * gl_Color.a);

      vec4 bg_color = texture2D(background_tex, vec2((float(gl_FragCoord.x + offset.x)/800.0),
                                                     (float(gl_FragCoord.y + offset.y)/600.0)) 
                                * vec2(800.0/1024.0, 600.0/1024.0));
      bg_color = mix(bg_color, vec4(noise_color.r*0.5, noise_color.r, 1, 1), clamp(pow(gl_Color.a, 2.0), 0, 1.0));
      gl_FragColor = mix(bg_color,
                         gl_Color,
                         clamp(pow(gl_Color.a, 2.0), 0, 1.0));
    }
  else
    {
      uniform sampler2D background_tex;
      uniform sampler2D noise_tex;
      varying vec2      tex_coord;
      varying vec2      tex_coord2;

        vec4 uv_col  = texture2D(noise_tex, tex_coord2.xy);
        vec4 uv_col2 = texture2D(noise_tex, tex_coord2.xy + vec2(0.3, 0.3));

        //  vec2 pos = tex_coord.xy;
        vec2 pos = tex_coord.xy + ((uv_col.rg-0.5) * (uv_col.b*.05));
        vec2 pos2 = pos + vec2(0.3, 0.3);
        vec4 tex_col;
        vec4 tex_col2;

        // blur
        for(int i = -2; i <= 2; ++i) {
          tex_col += texture2D(background_tex, pos.xy + vec2(0, i*(1/128.0)));
          tex_col += texture2D(background_tex, pos.xy + vec2(i*(1/128.0), 0));
        }
        tex_col *= 1.0/5.0;

        float r = -tex_coord.y;
        tex_col = mix(tex_col, vec4(r, r*0.5, 0, 1), r);

        tex_col2= texture2D(background_tex, pos.xy);

        gl_FragColor = mix(tex_col, tex_col2, clamp(pow(uv_col2.b*2, 1.5)/2.0, 0, 1.0));
    }
}

/* EOF */
