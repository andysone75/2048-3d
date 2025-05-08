#version 100
precision mediump float;

varying vec2 TexCoords;

uniform sampler2D tex;
uniform vec3 color;

void main()
{
    vec4 result = texture2D(tex, TexCoords);
    result.rgb *= color;
    gl_FragColor = result;
} 