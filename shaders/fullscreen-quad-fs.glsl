#version 100
precision mediump float;

varying vec2 TexCoord;

uniform sampler2D source;
uniform sampler2D ssaoMap;

uniform float ssaoPower;

void main() {
    float ssao = texture2D(ssaoMap, TexCoord).r;
    ssao = mix(1.0 - ssaoPower, 1.0, ssao);

    vec4 result = vec4(texture2D(source, TexCoord).rgb, 1.0);
    result.rgb *= ssao;

    gl_FragColor = result;
}
