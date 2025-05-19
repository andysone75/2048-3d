#version 100
precision mediump float;

varying vec2 TexCoord;

uniform sampler2D source;
uniform sampler2D shading;

void main() {
    vec4 color = texture2D(source, TexCoord);
    float shade = 1. - texture2D(shading, TexCoord).r;

    color.rgb *= shade;

    gl_FragColor = color;
    //gl_FragColor = vec4(vec3(shade), 1);
}