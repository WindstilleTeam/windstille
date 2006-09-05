uniform sampler2D texture;

void main()
{
  vec4 color = texture2D(texture, gl_TexCoord[0].xy);
  float f = (color.r + color.b + color.g)/3.0;

  float alpha = (float(gl_FragCoord.x)/800.0);

  if (f > 0.5) 
    gl_FragColor = mix(vec4(1,1,1,1), color, alpha);
  else if (f > 0.3)
    gl_FragColor = mix(vec4(0.5,0.5,0.5,1.0), color, alpha);
  else
    gl_FragColor = mix(vec4(0,0,0,1), color, alpha);
}

/* EOF */
