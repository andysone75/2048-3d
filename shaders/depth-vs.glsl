#version 100

attribute vec3 aPosition;

uniform mat4 model;
uniform mat4 viewProj;

varying vec3 fragPos;

void main() {
    gl_Position = viewProj * model * vec4(aPosition, 1.0);
}
