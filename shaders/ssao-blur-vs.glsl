#version 100

attribute vec2 vertexTexCoord;
attribute vec3 vertexPosition;

uniform mat4 mvp;

varying vec2 fragUv;

void main() {
    fragUv = vertexTexCoord;
    gl_Position = mvp * vec4(vertexPosition, 1.0);
}
