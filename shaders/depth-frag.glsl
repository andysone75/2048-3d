#version 100
precision highp float;

varying vec3 fragPos;
uniform mat4 viewProj;

void main() {
	vec4 fragPosLightSpace = viewProj * vec4(fragPos, 1.);
	vec3 projCoords = fragPosLightSpace.xyz / fragPosLightSpace.w;
	projCoords.xyz = (projCoords.xyz + 1.) * .5;
	float depth = projCoords.z;
}