#version 100
precision mediump float;

varying vec3 fragPos;
varying vec3 normal;

uniform vec3 lightPos;
uniform vec3 viewPos;

void main() {
    vec3 norm = normalize(normal);
    vec3 lightDir = normalize(lightPos - fragPos);
    vec3 viewDir = normalize(viewPos - fragPos);
    vec3 reflectDir = reflect(-lightDir, norm);

    // Ambient
    float ambient = 0.2;

    // Diffuse
    float diff = max(dot(norm, lightDir), 0.0);

    // Specular
    float spec = pow(max(dot(viewDir, reflectDir), 0.0), 16.0);

    vec3 color = (ambient + diff + spec) * vec3(1.0, 0.4, 0.4);
    gl_FragColor = vec4(color, 1.0);
}
