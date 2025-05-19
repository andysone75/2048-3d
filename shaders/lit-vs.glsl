#version 100
attribute vec3 aPosition;
attribute vec3 aNormal;
attribute vec4 aColor;

varying vec3 vPos;
varying vec3 vNormal;
varying vec4 vColor;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;

// lighting
uniform vec3 lightDir;
varying float diff;

void main() {
    vPos = vec3(model * vec4(aPosition, 1.0));
    vNormal = normalize(mat3(model) * aNormal);
    vColor = aColor;
    gl_Position = projection * view * vec4(vPos, 1.0);

    // lighting
    vec3 norm = normalize(vNormal);
    vec3 lightDirection = normalize(-lightDir);
    diff = max(dot(norm, lightDirection), 0.0);
}