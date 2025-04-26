#version 100
attribute vec3 vertexPosition;
attribute vec3 vertexNormal;

uniform mat4 mvp;
uniform mat4 matModel;

varying vec3 fragPos;
varying vec3 normal;

void main() {
    fragPos = vec3(matModel * vec4(vertexPosition, 1.0));
    normal = mat3(matModel) * vertexNormal;
    gl_Position = mvp * vec4(vertexPosition, 1.0);
}
