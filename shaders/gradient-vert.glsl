#version 100
attribute vec3 aPosition;
attribute vec3 aNormal;
attribute vec2 aUv;

uniform mat4 MVP;
uniform mat4 model;

varying vec3 fragPos;
varying vec3 fragLocalPos;
varying vec3 normal;
varying vec2 fragTexCoord;

void main() {
    fragPos = vec3(model * vec4(aPosition, 1.0));
    fragLocalPos = aPosition;
    normal = mat3(model) * aNormal;
    fragTexCoord = aUv;
    gl_Position = MVP * vec4(aPosition, 1.0);
}
