#version 100

attribute vec4 vertex; // <vec2 pos, vec2 tex>
varying vec2 TexCoords;
uniform mat4 projection;

void main()
{
    gl_Position = projection * vec4(vertex.xy, 0.0, 1.0);
    TexCoords = vertex.zw;
} 