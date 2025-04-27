#version 100
precision mediump float;

varying vec2 fragTexCoord;
uniform sampler2D texture0;

void main() {
	vec2 flippedTexCoord = vec2(fragTexCoord.x, 1.0 - fragTexCoord.y);
	vec4 textureColor = texture2D(texture0, flippedTexCoord);
	gl_FragColor = textureColor;
}