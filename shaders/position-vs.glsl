#version 100

attribute vec3 aPosition;

uniform mat4 model;
uniform mat4 view;
uniform mat4 proj;

varying vec3 fragPos;

void main() {
    fragPos = (view * model * vec4(aPosition, 1.0)).xyz;
    gl_Position = proj * view * model * vec4(aPosition, 1.0);
}
