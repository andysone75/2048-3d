#version 100

attribute vec3 aPosition;

uniform mat4 model;
uniform mat4 viewProj;

varying vec3 fragPos;

void main() {
    fragPos = vec3(model * vec4(aPosition, 1.0));
    gl_Position = viewProj * model * vec4(aPosition, 1.0);
}
