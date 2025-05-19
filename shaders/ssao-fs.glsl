#version 100
precision highp float;

varying vec2 TexCoord;
const int samplesCount = 16;

uniform sampler2D gPosition;
uniform sampler2D gNormal;
uniform sampler2D noiseTex;

uniform mat4 view;
uniform mat4 projection;
uniform float ssaoRadius;
uniform float ssaoBias;
uniform float ssaoPower;
uniform vec3 samples[samplesCount];
uniform vec2 resolution;

void main() {
	vec3 vPos = texture2D(gPosition, TexCoord).rgb;
	if (vPos.z > -.001) {
		gl_FragColor = vec4(0);
		return;
	}
	vec3 vNormal = texture2D(gNormal, TexCoord).rgb;

	vec2 noiseScale = resolution / 4.;
	vec3 randomVec = texture2D(noiseTex, TexCoord * noiseScale).xyz;

	vec3 tangent = normalize(randomVec - vNormal * dot(randomVec, vNormal));
	vec3 bitangent = cross(vNormal, tangent);
	mat3 TBN = mat3(tangent, bitangent, vNormal);

	float occlusion = 0.0;
	for (int i = 0; i < samplesCount; i++) {
		vec3 samp = TBN * samples[i];
		samp = vPos + samp * ssaoRadius;
		
		vec4 offset = vec4(samp, 1.0);
		offset = projection * offset;
		offset.xyz /= offset.w;
		offset.xyz = offset.xyz * 0.5 + 0.5;

		float sampleDepth = texture2D(gPosition, offset.xy).z;
		float rangeCheck = smoothstep(0.0, 1.0, ssaoRadius / abs(vPos.z - sampleDepth));
		occlusion += (sampleDepth >= samp.z + ssaoBias ? 1.0 : 0.0) * rangeCheck;
	}

	occlusion = occlusion / float(samplesCount) * ssaoPower;

    gl_FragColor = vec4(vec3(occlusion), 1);
}