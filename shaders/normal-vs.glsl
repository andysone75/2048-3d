#version 100

attribute vec3 aPosition;
attribute vec3 aNormal;

uniform mat4 model;
uniform mat4 view;
uniform mat4 proj;

varying vec3 normal;

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
    mat3 normalMatrix = transpose(inverse(mat3(view * model)));
    normal = normalMatrix * aNormal;
    gl_Position = proj * view * model * vec4(aPosition, 1.0);
}
