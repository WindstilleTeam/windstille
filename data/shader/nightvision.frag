uniform sampler2D texture;

/* Simple shader that converts the images brightness to a gradient: [black, green, white]
   FIXME: Using a sampler1D would be more flexible, maybe faster
 */
void main()
{
  int kernel = 2;
  vec4 color;

  for(int y = -kernel; y <= kernel; ++y)
    color += texture2D(texture, gl_TexCoord[0].xy + vec2(float(0)/128.0, float(y)/128.0));

  for(int x = -kernel; x <= kernel; ++x)
    color += texture2D(texture, gl_TexCoord[0].xy + vec2(float(x)/128.0, float(0)/128.0));

  color *= 0.5 / (2.0*float(kernel)+1.0);

  float brightness = (color.r + color.g + color.b) / 3.0;

  if (brightness < 0.5)
  {
    gl_FragColor = vec4(0.0, 2.0 * brightness, 0.0, color.a);
  }
  else
  {
    gl_FragColor = vec4(2.0*brightness - 1.0, 1.0, 2.0*brightness - 1.0, color.a);
  }
}

/* EOF */
