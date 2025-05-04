#version 100
precision highp float;

// Possible solution if highp has low compatibility with old mobiles
//#ifdef GL_FRAGMENT_PRECISION_HIGH
//    precision highp float;
//#else
//    precision mediump float;
//#endif

const int MAX_COLORS = 8;

varying vec3 vPos;
varying vec3 vNormal;

varying vec3 fragLocalPos;
varying vec2 fragTexCoord;

// shadow map
uniform vec3 lightDir;
uniform vec3 viewPos;

// gradient
uniform float time;
uniform vec3 color0;
uniform vec3 color1;

// shadow mapping
uniform mat4 lightViewProj;
uniform int shadowMapResolution;
uniform sampler2D shadowMap;

// ssao
const float ssaoRadius = 0.15;
const float ssaoBias = 0.03;
const int samplesCount = 64;

uniform sampler2D gPosition;
uniform sampler2D texNoise;
uniform vec3 samples[samplesCount];

varying vec2 noiseScale;
varying mat3 normalMatrix;

uniform mat4 view;
uniform mat4 projection;

vec3 mod289(vec3 x)
{
  return x - floor(x * (1.0 / 289.0)) * 289.0;
}

vec4 mod289(vec4 x)
{
  return x - floor(x * (1.0 / 289.0)) * 289.0;
}

vec4 permute(vec4 x)
{
  return mod289(((x*34.0)+10.0)*x);
}

vec4 taylorInvSqrt(vec4 r)
{
  return 1.79284291400159 - 0.85373472095314 * r;
}

vec3 fade(vec3 t) {
  return t*t*t*(t*(t*6.0-15.0)+10.0);
}

// Classic Perlin noise
float cnoise(vec3 P)
{
  vec3 Pi0 = floor(P); // Integer part for indexing
  vec3 Pi1 = Pi0 + vec3(1.0); // Integer part + 1
  Pi0 = mod289(Pi0);
  Pi1 = mod289(Pi1);
  vec3 Pf0 = fract(P); // Fractional part for interpolation
  vec3 Pf1 = Pf0 - vec3(1.0); // Fractional part - 1.0
  vec4 ix = vec4(Pi0.x, Pi1.x, Pi0.x, Pi1.x);
  vec4 iy = vec4(Pi0.yy, Pi1.yy);
  vec4 iz0 = Pi0.zzzz;
  vec4 iz1 = Pi1.zzzz;

  vec4 ixy = permute(permute(ix) + iy);
  vec4 ixy0 = permute(ixy + iz0);
  vec4 ixy1 = permute(ixy + iz1);

  vec4 gx0 = ixy0 * (1.0 / 7.0);
  vec4 gy0 = fract(floor(gx0) * (1.0 / 7.0)) - 0.5;
  gx0 = fract(gx0);
  vec4 gz0 = vec4(0.5) - abs(gx0) - abs(gy0);
  vec4 sz0 = step(gz0, vec4(0.0));
  gx0 -= sz0 * (step(0.0, gx0) - 0.5);
  gy0 -= sz0 * (step(0.0, gy0) - 0.5);

  vec4 gx1 = ixy1 * (1.0 / 7.0);
  vec4 gy1 = fract(floor(gx1) * (1.0 / 7.0)) - 0.5;
  gx1 = fract(gx1);
  vec4 gz1 = vec4(0.5) - abs(gx1) - abs(gy1);
  vec4 sz1 = step(gz1, vec4(0.0));
  gx1 -= sz1 * (step(0.0, gx1) - 0.5);
  gy1 -= sz1 * (step(0.0, gy1) - 0.5);

  vec3 g000 = vec3(gx0.x,gy0.x,gz0.x);
  vec3 g100 = vec3(gx0.y,gy0.y,gz0.y);
  vec3 g010 = vec3(gx0.z,gy0.z,gz0.z);
  vec3 g110 = vec3(gx0.w,gy0.w,gz0.w);
  vec3 g001 = vec3(gx1.x,gy1.x,gz1.x);
  vec3 g101 = vec3(gx1.y,gy1.y,gz1.y);
  vec3 g011 = vec3(gx1.z,gy1.z,gz1.z);
  vec3 g111 = vec3(gx1.w,gy1.w,gz1.w);

  vec4 norm0 = taylorInvSqrt(vec4(dot(g000, g000), dot(g010, g010), dot(g100, g100), dot(g110, g110)));
  vec4 norm1 = taylorInvSqrt(vec4(dot(g001, g001), dot(g011, g011), dot(g101, g101), dot(g111, g111)));

  float n000 = norm0.x * dot(g000, Pf0);
  float n010 = norm0.y * dot(g010, vec3(Pf0.x, Pf1.y, Pf0.z));
  float n100 = norm0.z * dot(g100, vec3(Pf1.x, Pf0.yz));
  float n110 = norm0.w * dot(g110, vec3(Pf1.xy, Pf0.z));
  float n001 = norm1.x * dot(g001, vec3(Pf0.xy, Pf1.z));
  float n011 = norm1.y * dot(g011, vec3(Pf0.x, Pf1.yz));
  float n101 = norm1.z * dot(g101, vec3(Pf1.x, Pf0.y, Pf1.z));
  float n111 = norm1.w * dot(g111, Pf1);

  vec3 fade_xyz = fade(Pf0);
  vec4 n_z = mix(vec4(n000, n100, n010, n110), vec4(n001, n101, n011, n111), fade_xyz.z);
  vec2 n_yz = mix(n_z.xy, n_z.zw, fade_xyz.y);
  float n_xyz = mix(n_yz.x, n_yz.y, fade_xyz.x); 
  return 2.2 * n_xyz;
}

void main() {
    // lighting
    vec3 norm = normalize(vNormal);
    vec3 lightDirection = normalize(-lightDir);
    vec3 viewDir = normalize(viewPos - vPos);
    vec3 reflectDir = reflect(-lightDirection, norm);

    float ambient = 0.65;
    float diff = max(dot(norm, lightDirection), 0.0);
    float spec = pow(max(dot(viewDir, reflectDir), 0.0), 16.0);

    // gradient
    float noiseScaleGrad = .9;
    vec3 noisePos = fragLocalPos + vec3(cos(time * .6), sin(time * .6), time * .1);
    float noise = cnoise(vec3(noisePos) * noiseScaleGrad);
    noise = (noise + 1.) * .5;

    // === shadow mapping ===
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

    vec4 fp = projection * vec4(fragPos, 1.0);
    fp.xyz /= fp.w;
    fp.xyz = fp.xyz * 0.5 + 0.5;

	vec3 randomVec = normalize(texture2D(texNoise, fp.xy * noiseScale).xyz);

	vec3 tangent = normalize(randomVec - normal * dot(randomVec, normal));
	vec3 bitangent = cross(normal, tangent);
	mat3 TBN = mat3(tangent, bitangent, normal);

	float occlusion = 0.0;
	for (int i = 0; i < samplesCount; i++) {
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

    float ssaoPower = 0.8;

	occlusion = 1.0 - (occlusion / float(samplesCount));
    occlusion = mix(1.0 - ssaoPower, 1.0, occlusion);

    vec3 result = (ambient + diff + spec * (1.0 - shadow)) * mix(color0, color1, noise) * mix(1.0, 0.9, shadow) * occlusion;
    gl_FragColor = vec4(result, 1.0);
}