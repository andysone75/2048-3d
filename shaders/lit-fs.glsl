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
varying float diff;

// shadow mapping
uniform mat4 lightViewProj;
uniform int shadowMapResolution;
uniform sampler2D shadowMap;
uniform float shadowPower;

void main() {
    // shadow mapping
    vec4 fragPosLightSpace = lightViewProj * vec4(vPos, 1.);
	vec3 projCoords = fragPosLightSpace.xyz / fragPosLightSpace.w;
	projCoords.xyz = (projCoords.xyz + 1.) * .5;
    vec2 sampleCoords = projCoords.xy;
    float curDepth = projCoords.z;
    
    float start = 0.00251623; float slope = -0.0022182;
    float bias = start + slope * (1.0 - dot(normalize(vNormal), -lightDir));
    float sampleDepth = texture2D(shadowMap, sampleCoords).r;
    float shadow = curDepth - bias > sampleDepth ? 1.0 : 0.0;

    //float shading = clamp((1.0 - diff) * shadingPower + shadow * shadowPower + occlusion, 0.0, 1.0);
    float shading = clamp((1.0 - diff) * shadingPower + shadow * shadowPower, 0.0, 1.0); // Temporary remove SSAO
    vec3 result = vColor.rgb * (1.0 - shading);

    gl_FragColor = vec4(result, 1.0);
}
