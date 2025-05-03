#version 100
precision highp float;

//const float radius = 0.079;
const float radius = 0.1;
const float bias = 0.015;
const int samplesCount = 64;

uniform sampler2D gPosition;
uniform sampler2D gNormal;
uniform sampler2D texNoise;
uniform vec2 resolution;
uniform vec3 samples[samplesCount];
uniform mat4 projection;

varying vec2 TexCoord;

void main() {
	vec2 noiseScale = resolution / 4.0;

	vec3 fragPos = texture2D(gPosition, TexCoord).xyz;
	vec3 normal = normalize(texture2D(gNormal, TexCoord).rgb);
	vec3 randomVec = normalize(texture2D(texNoise, TexCoord * noiseScale).xyz);

	vec3 tangent = normalize(randomVec - normal * dot(randomVec, normal));
	vec3 bitangent = cross(normal, tangent);
	mat3 TBN = mat3(tangent, bitangent, normal);

	float occlusion = 0.0;
	for (int i = 0; i < samplesCount; i++) {
		vec3 samp = TBN * samples[i];
		samp = fragPos + samp * radius;
		
		vec4 offset = vec4(samp, 1.0);
		offset = projection * offset;
		offset.xyz /= offset.w;
		offset.xyz = offset.xyz * 0.5 + 0.5;
		
		float sampleDepth = texture2D(gPosition, offset.xy).z;
        float rangeCheck = smoothstep(0.0, 1.0, radius / abs(fragPos.z - sampleDepth));
		occlusion += (sampleDepth >= samp.z + bias ? 1.0 : 0.0) * rangeCheck;
	}

	occlusion = 1.0 - (occlusion / float(samplesCount));
	gl_FragColor = vec4(vec3(occlusion), 1.0);
}