uniform sampler2D texture;
uniform sampler2D noise;
uniform float time;

void main()
{
  vec4 noise1 = texture2D(noise, gl_TexCoord[0].xy*2.0 + vec2(0, sin(time)));
  vec4 noise2 = texture2D(noise, gl_TexCoord[0].xy*2.0 + vec2(-time*1.1));
  vec2 offset = (noise1.rg + noise2.rg - 1.0) * 0.1;

  float dx = gl_FragCoord.x/800.0 - 0.5;
  float dy = gl_FragCoord.y/600.0 - 0.5;

  float d = sqrt(dx*dx + dy*dy)/3.0;
  offset *= d;
  vec4 color = texture2D(texture, gl_TexCoord[0].xy + offset);

  gl_FragColor = mix(color, vec4(1.0, 1.0, 1.0, 1.0), length(offset*10));
}

/* EOF */
