#version 100
attribute vec3 vertexPosition;
attribute vec3 vertexNormal;
attribute vec2 vertexTexCoord;

uniform mat4 mvp;
uniform mat4 matModel;

varying vec3 fragPos;
varying vec3 fragLocalPos;
varying vec3 normal;
varying vec2 fragTexCoord;

void main() {
    fragPos = vec3(matModel * vec4(vertexPosition, 1.0));
    fragLocalPos = vertexPosition;
    normal = mat3(matModel) * vertexNormal;
    fragTexCoord = vertexTexCoord;
    gl_Position = mvp * vec4(vertexPosition, 1.0);
}
