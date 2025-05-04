#version 100
attribute vec3 aPosition;
attribute vec3 aNormal;
attribute vec2 aUv;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;

varying vec3 vPos;
varying vec3 vNormal;

varying vec3 fragLocalPos;
varying vec2 fragTexCoord;

// ssao
uniform vec2 resolution;
varying vec2 noiseScale;
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
    fragLocalPos = aPosition;
    vNormal = normalize(mat3(model) * aNormal);
    fragTexCoord = aUv;
    gl_Position = projection * view * vec4(vPos, 1.0);
    // ssao
    noiseScale = resolution / 4.0;
    normalMatrix = transpose(inverse(mat3(view * model)));
}
