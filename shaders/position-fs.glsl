#version 100
precision highp float;

varying vec3 fragPos;

void main() {
	gl_FragColor = vec4(fragPos, 1.0);
}