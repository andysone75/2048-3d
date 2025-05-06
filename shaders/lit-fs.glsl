#version 100
precision highp float;

varying vec3 vPos;
varying vec3 vNormal;
varying vec4 vColor;

uniform mat4 view;
uniform mat4 projection;

// lighting
uniform vec3 lightDir;
uniform float shadingPower;

// shadow mapping
uniform mat4 lightViewProj;
uniform int shadowMapResolution;
uniform sampler2D shadowMap;
uniform float shadowPower;

// ssao
const int samplesCount = 128;

uniform float ssaoRadius;
uniform float ssaoBias;
uniform float ssaoPower;
uniform sampler2D gPosition;
uniform vec3 samples[samplesCount];

varying mat3 normalMatrix;

void main() {
    // lighting
    vec3 norm = normalize(vNormal);
    vec3 lightDirection = normalize(-lightDir);
    float diff = max(dot(norm, lightDirection), 0.0);

    // shadow mapping
    vec4 fragPosLightSpace = lightViewProj * vec4(vPos, 1.);
	vec3 projCoords = fragPosLightSpace.xyz / fragPosLightSpace.w;
	projCoords.xyz = (projCoords.xyz + 1.) * .5;
    vec2 sampleCoords = projCoords.xy;
    float curDepth = projCoords.z;
    
    float start = 0.00251623; float slope = -0.0022182;
    float bias = start + slope * (1.0 - dot(norm, -lightDir));
    float sampleDepth = texture2D(shadowMap, sampleCoords).r;
    float shadow = curDepth - bias > sampleDepth ? 1.0 : 0.0;

    // ssao
	vec3 fragPos = (view * vec4(vPos, 1.0)).xyz;
	vec3 normal = normalMatrix * vNormal;
	vec3 randomVec = normalize(vec3(1, .1, 0));

	vec3 tangent = normalize(randomVec - normal * dot(randomVec, normal));
	vec3 bitangent = cross(normal, tangent);
	mat3 TBN = mat3(tangent, bitangent, normal);

	float occlusion = 0.0;
	for (int i = 0; i < 128; i++) {
		vec3 samp = TBN * samples[i];
		samp = fragPos + samp * ssaoRadius;
		
		vec4 offset = vec4(samp, 1.0);
		offset = projection * offset;
		offset.xyz /= offset.w;
		offset.xyz = offset.xyz * 0.5 + 0.5;

		float sampleDepth = texture2D(gPosition, offset.xy).z;
		float rangeCheck = smoothstep(0.0, 1.0, ssaoRadius / abs(fragPos.z - sampleDepth));
		occlusion += (sampleDepth >= samp.z + ssaoBias ? 1.0 : 0.0) * rangeCheck;
	}

	occlusion = occlusion / float(samplesCount);

    //float shading = clamp((1.0 - diff) * shadingPower + shadow * shadowPower + occlusion * ssaoPower, 0.0, 1.0);
    float shading = clamp((1.0 - diff) * shadingPower + shadow * shadowPower, 0.0, 1.0);
    vec3 result = vColor.rgb * (1.0 - shading);

    gl_FragColor = vec4(result, 1.0);
}
