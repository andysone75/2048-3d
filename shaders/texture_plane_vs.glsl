#version 100

attribute vec3 aPosition;
attribute vec2 aUv;

uniform mat4 mvp;

varying vec2 fragTexCoord;

void main () {
	fragTexCoord = aUv;
	gl_Position = mvp * vec4(aPosition, 1.0);
}