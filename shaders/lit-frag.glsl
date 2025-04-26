#version 100
precision mediump float;

varying vec3 fragPos;
varying vec3 normal;

uniform vec3 lightDir;
uniform vec3 viewPos;
uniform vec3 objectColor;

void main() {
    vec3 norm = normalize(normal);
    vec3 lightDirection = normalize(-lightDir);
    vec3 viewDir = normalize(viewPos - fragPos);
    vec3 reflectDir = reflect(-lightDirection, norm);

    float ambient = 0.5;
    float diff = max(dot(norm, lightDirection), 0.0);
    float spec = pow(max(dot(viewDir, reflectDir), 0.0), 16.0);

    vec3 result = (ambient + diff + spec) * objectColor;
    gl_FragColor = vec4(result, 1.0);
}