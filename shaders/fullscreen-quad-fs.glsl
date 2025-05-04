#version 100
precision mediump float;

varying vec2 TexCoord;

uniform sampler2D source;

void main() {
    gl_FragColor = texture2D(source, TexCoord);
}