#version 100
precision mediump float;

varying vec2 TexCoords;

uniform sampler2D tex;
uniform vec4 color;

void main()
{
    vec4 result = texture2D(tex, TexCoords);
    result *= color;
    gl_FragColor = result;
} 