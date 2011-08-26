uniform sampler2D texture;

/* The border shader puts a pixel border around a sprite */
void main()
{
  //ivec2 size = textureSize2D(texture, 0);

  float pixel_step = 0.02;
  vec4 right = texture2D(texture, gl_TexCoord[0].xy + vec2( pixel_step,    0));
  vec4 up    = texture2D(texture, gl_TexCoord[0].xy + vec2(   0,  pixel_step));
  vec4 down  = texture2D(texture, gl_TexCoord[0].xy + vec2(   0, -pixel_step));
  vec4 left  = texture2D(texture, gl_TexCoord[0].xy + vec2(-pixel_step,    0));

  if ((right.a == 0.0 && left.a  != 0.0) ||
      (left.a  == 0.0 && right.a != 0.0) ||
      (down.a  == 0.0 && up.a    != 0.0) ||
      (up.a    == 0.0 && down.a  != 0.0))
  {
    gl_FragColor = vec4(1.0,1.0,1.0,1.0);
  }
  else
  {
    gl_FragColor = texture2D(texture, gl_TexCoord[0].xy);
  }
}

/* EOF */
