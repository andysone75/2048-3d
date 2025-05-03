#version 100
attribute vec3 aPosition;
attribute vec3 aNormal;
attribute vec4 aColor;

uniform mat4 model;
uniform mat4 MVP;

varying vec3 vPos;
varying vec3 vNormal;
varying vec4 vColor;

void main() {
    vPos = vec3(model * vec4(aPosition, 1.0));
    vNormal = normalize(mat3(model) * aNormal);
    vColor = aColor;
    gl_Position = MVP * vec4(aPosition, 1.0);
}