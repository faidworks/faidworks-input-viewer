#version 130

uniform sampler2D texture;
uniform vec4 tintColor;

void main()
{
    vec4 texel = texture2D(texture, gl_TexCoord[0].xy);
    gl_FragColor = texel * tintColor * gl_Color;
}
