#version 100

attribute vec3 aPosition;
attribute vec3 aNormal;

uniform mat4 model;
uniform mat4 view;
uniform mat4 proj;

varying vec3 vNormal;

void main() {
    vNormal = normalize((view * model * vec4(aNormal, 0.0)).xyz);
    gl_Position = proj * view * model * vec4(aPosition, 1.0);
}
