vec4 brightness(vec4 color, float factor)
{
        return vec4((color.r + factor),
                    (color.g + factor),
                    (color.b + factor),
                    color.a);
}

vec4 contrast(vec4 color, float factor)
{
        return vec4((color.r - 0.5) * factor + 0.5,
                    (color.g - 0.5) * factor + 0.5,
                    (color.b - 0.5) * factor + 0.5,
                    color.a);
}
