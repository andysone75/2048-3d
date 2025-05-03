#version 100
precision highp float;

uniform sampler2D ssaoTex;
uniform sampler2D texture0;
uniform vec2 texSize;

varying vec2 fragUv;

void main() {
	vec2 texelSize = 1.0 / texSize;
	float result = 0.0;
	for (int x = -2; x < 2; x++) {
		for (int y = -2; y < 2; y++) {
			vec2 offset = vec2(float(x), float(y)) * texelSize;
			result += texture2D(ssaoTex, fragUv + offset).r;
		}
	}
	float occlusion = result / (4.0 * 4.0);
	gl_FragColor = texture2D(texture0, fragUv) * occlusion;
	//gl_FragColor = vec4(vec3(occlusion), 1.0);
	//gl_FragColor = texture2D(ssaoTex, fragUv);
}