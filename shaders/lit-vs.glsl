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

// ssao
varying mat3 normalMatrix;

mat3 transpose(mat3 m) {
    return mat3(
        m[0][0], m[1][0], m[2][0],
        m[0][1], m[1][1], m[2][1],
        m[0][2], m[1][2], m[2][2]
    );
}

mat3 inverse(mat3 m) {
    float det = m[0][0] * (m[1][1] * m[2][2] - m[1][2] * m[2][1]) -
                m[0][1] * (m[1][0] * m[2][2] - m[1][2] * m[2][0]) +
                m[0][2] * (m[1][0] * m[2][1] - m[1][1] * m[2][0]);

    float invDet = 1.0 / det;

    mat3 res;
    res[0][0] = (m[1][1] * m[2][2] - m[1][2] * m[2][1]) * invDet;
    res[0][1] = (m[0][2] * m[2][1] - m[0][1] * m[2][2]) * invDet;
    res[0][2] = (m[0][1] * m[1][2] - m[0][2] * m[1][1]) * invDet;
    res[1][0] = (m[1][2] * m[2][0] - m[1][0] * m[2][2]) * invDet;
    res[1][1] = (m[0][0] * m[2][2] - m[0][2] * m[2][0]) * invDet;
    res[1][2] = (m[0][2] * m[1][0] - m[0][0] * m[1][2]) * invDet;
    res[2][0] = (m[1][0] * m[2][1] - m[1][1] * m[2][0]) * invDet;
    res[2][1] = (m[0][1] * m[2][0] - m[0][0] * m[2][1]) * invDet;
    res[2][2] = (m[0][0] * m[1][1] - m[0][1] * m[1][0]) * invDet;
    return res;
}

void main() {
    vPos = vec3(model * vec4(aPosition, 1.0));
    vNormal = normalize(mat3(model) * aNormal);
    vColor = aColor;
    gl_Position = projection * view * vec4(vPos, 1.0);

    // lighting
    vec3 norm = normalize(vNormal);
    vec3 lightDirection = normalize(-lightDir);
    diff = max(dot(norm, lightDirection), 0.0);

    // ssao
    normalMatrix = transpose(inverse(mat3(view * model)));
}