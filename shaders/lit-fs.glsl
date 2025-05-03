#version 100
precision highp float;

varying vec3 vPos;
varying vec3 vNormal;
varying vec4 vColor;

uniform vec3 lightDir;
uniform vec3 viewPos;

// shadow mapping
uniform mat4 lightViewProj;
uniform int shadowMapResolution;
uniform sampler2D shadowMap;

void main() {
    vec3 norm = normalize(vNormal);
    vec3 lightDirection = normalize(-lightDir);
    vec3 viewDir = normalize(viewPos - vPos);
    vec3 reflectDir = reflect(-lightDirection, norm);

    float ambient = 0.4;
    float diff = max(dot(norm, lightDirection), 0.0);
    float spec = pow(max(dot(viewDir, reflectDir), 0.0), 16.0);

    // shadow mapping
    vec4 fragPosLightSpace = lightViewProj * vec4(vPos, 1.);
	vec3 projCoords = fragPosLightSpace.xyz / fragPosLightSpace.w;
	projCoords.xyz = (projCoords.xyz + 1.) * .5;
    vec2 sampleCoords = projCoords.xy;
    float curDepth = projCoords.z;
    
    float start = 0.00251623; float slope = -0.0022182;
    //float start = 0.00829533; float slope = -0.00218537; // Good for PCF
    float bias = start + slope * (1.0 - dot(norm, -lightDir));
    
    // Non PCF
    float sampleDepth = texture2D(shadowMap, sampleCoords).r;
    float shadow = curDepth - bias > sampleDepth ? 1.0 : 0.0;
    
    // PCF (percentage-closer filtering) algorithm
    //int shadowCounter = 0;
    //const int numSamples = 9;
    //vec2 texelSize = vec2(1.0/float(shadowMapResolution));
    //for (int x = -1; x <= 1; x++) {
    //    for (int y = -1; y <= 1; y++) {
    //        float sampleDepth = texture2D(shadowMap, sampleCoords + texelSize * vec2(x,y)).r;
    //        if (curDepth - bias > sampleDepth) shadowCounter++;
    //    }
    //}
    //float shadow = float(shadowCounter)/float(numSamples);

    vec3 result = (ambient + diff + spec * (1.0 - shadow)) * vColor.rgb * mix(1.0, 0.575, shadow);
    gl_FragColor = vec4(result, 1.0);
}
