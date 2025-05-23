#version 100
precision mediump float;

varying vec2 TexCoords;

uniform sampler2D text;
uniform vec4 textColor;

void main()
{    
    vec4 sampled = vec4(1.0, 1.0, 1.0, texture2D(text, TexCoords).r);
    gl_FragColor = textColor * sampled;
} 