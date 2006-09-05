// -*- c++ -*-

uniform sampler2DRect background;

void main()
{
  if (gl_FragCoord.x < 300)
    {
      gl_FragColor = texture2DRect(background, gl_TexCoord[0].xy);
    }
  else
    {
      gl_FragColor = vec4(1.0, 1.0, 0.0, 1.0);
    }
}

/* EOF */
