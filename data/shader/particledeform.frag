// -*- c++ -*-

uniform sampler2DRect screen;
uniform sampler2DRect particles;

void main()
{
  vec3 offset  = texture2DRect(particles, gl_FragCoord.xy).rgb;
  vec3 color   = gl_FragCoord.rgb;
  gl_FragColor = texture2DRect(screen, gl_FragCoord.xy + (offset.rg)* 50.0);
}

/* EOF */
