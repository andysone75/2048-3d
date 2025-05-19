#version 100
precision mediump float;
  
varying vec2 TexCoord;
  
uniform sampler2D ssaoInput;
uniform vec2 texSize;

void main() {
    vec2 texelSize = 1.0 / texSize;
    float result = 0.0;
    for (int x = -2; x < 2; ++x) 
    {
        for (int y = -2; y < 2; ++y) 
        {
            vec2 offset = vec2(float(x), float(y)) * texelSize;
            result += texture2D(ssaoInput, TexCoord + offset).r;
        }
    }

    gl_FragColor = vec4(vec3(result / (4.0 * 4.0)), 1.0);
    //gl_FragColor = texture2D(ssaoInput, TexCoord);
}  