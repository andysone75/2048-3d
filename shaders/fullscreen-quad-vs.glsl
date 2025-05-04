#version 100

attribute vec2 aPos;
attribute vec2 aTexCoord;

varying vec2 TexCoord;

void main() {
    TexCoord = aTexCoord;
    gl_Position = vec4(aPos.xy, 0.0, 1.0);
}