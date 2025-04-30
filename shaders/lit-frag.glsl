#version 100
precision highp float;

varying vec3 fragPos;
varying vec3 normal;

uniform vec3 lightDir;
uniform vec3 viewPos;
uniform vec3 objectColor;
uniform sampler2D shadowMap;
uniform mat4 lightVp;
uniform int shadowMapResolution;

void main() {
    vec3 norm = normalize(normal);
    vec3 lightDirection = normalize(-lightDir);
    vec3 viewDir = normalize(viewPos - fragPos);
    vec3 reflectDir = reflect(-lightDirection, norm);

    float ambient = 0.5;
    float diff = max(dot(norm, lightDirection), 0.0);
    float spec = pow(max(dot(viewDir, reflectDir), 0.0), 16.0);
    
    vec4 fragPosLightSpace = lightVp * vec4(fragPos, 1.);
	vec3 projCoords = fragPosLightSpace.xyz / fragPosLightSpace.w;
	projCoords.xyz = (projCoords.xyz + 1.) * .5;
    vec2 sampleCoords = projCoords.xy;
    float curDepth = projCoords.z;

    //float bias = 0.0064 + -0.0026 * (1.0 - dot(normal, -lightDir));
    float bias = 0.019 + -0.016390 * (1.0 - dot(normal, -lightDir)); // Good for PCF
    
    // Non PCF
    //float sampleDepth = texture2D(shadowMap, sampleCoords).r;
    //float shadow = curDepth - bias > sampleDepth ? 1.0 : 0.0;
    
    // PCF (percentage-closer filtering) algorithm
    int shadowCounter = 0;
    const int numSamples = 9;
    vec2 texelSize = vec2(1.0/float(shadowMapResolution));
    for (int x = -1; x <= 1; x++) {
        for (int y = -1; y <= 1; y++) {
            float sampleDepth = texture2D(shadowMap, sampleCoords + texelSize * vec2(x,y)).r;
            if (curDepth - bias > sampleDepth) shadowCounter++;
        }
    }
    float shadow = float(shadowCounter)/float(numSamples);

    vec3 result = (ambient + diff + spec * (1.0 - shadow)) * objectColor * mix(1.0, 0.575, shadow);
    gl_FragColor = vec4(result, 1.0);
}