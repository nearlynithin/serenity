#version 330
// Input vertex attributes
in vec3 vertexPosition;
in vec2 vertexTexCoord;
in vec3 vertexNormal;
in mat4 instanceTransform;

// Input uniform values
uniform mat4 mvp;
uniform mat4 matNormal;
uniform mat4 lightVP;
uniform float time;
uniform vec3 camPos;
uniform vec3 camTarget;
uniform int renderPass; // 0 = shadow pass, 1 = main pass

// Output vertex attributes (to fragment shader)
out vec3 fragPosition;
out vec2 fragTexCoord;
out vec4 fragColor;
out vec3 fragNormal;
out vec4 fragPosLightSpace;

const float PI = 3.14159265358979323846f;

float rand(vec2 p) {
    return fract(sin(dot(p, vec2(127.1, 311.7))) * 43758.5453123);
}

float rand(float n) {
    return fract(sin(n) * 43758.5453123);
}

float noise(vec2 p, float freq) {
    float unit = 200.0 / freq;

    vec2 scaled = p / unit;
    vec2 ij = floor(scaled);
    vec2 xy = fract(scaled);

    xy = xy * xy * (3.0 - 2.0 * xy);

    float a = rand(ij + vec2(0.0, 0.0));
    float b = rand(ij + vec2(1.0, 0.0));
    float c = rand(ij + vec2(0.0, 1.0));
    float d = rand(ij + vec2(1.0, 1.0));

    float x1 = mix(a, b, xy.x);
    float x2 = mix(c, d, xy.x);
    return mix(x1, x2, xy.y);
}

float pNoise(vec2 p, int res){
	float persistance = .5;
	float n = 0.;
	float normK = 0.;
	float f = 16.;
	float amp = 1.;
	int iCount = 0;
	for (int i = 0; i<50; i++){
		n+=amp*noise(p, f);
		f*=2.;
		normK+=amp;
		amp*=persistance;
		if (iCount == res) break;
		iCount++;
	}
	float nf = n/normK;
	return nf*nf*nf*nf;
}

float simpleNoise(vec2 p) {
    return fract(sin(dot(p, vec2(12.9898, 78.233))) * 43758.5453);
}

float fbm(vec2 p, int octaves) {
    float value = 0.0;
    float amplitude = 1.0;
    float frequency = 1.0;
    float maxValue = 0.0;

    for(int i = 0; i < octaves; i++) {
        value += simpleNoise(p * frequency) * amplitude;
        maxValue += amplitude;
        amplitude *= 0.5;
        frequency *= 2.0;
    }

    return value / maxValue;
}

void main()
{
    vec3 scale = vec3(length(vec3(instanceTransform[0])),
                      length(vec3(instanceTransform[1])),
                      length(vec3(instanceTransform[2])));
    vec3 bladePos = vec3(instanceTransform[3]);
    vec2 windDir = normalize(vec2(1.0, -1.0));

    float phase = pNoise(bladePos.xz + vec2(time * 5.0 + time * 5.0), 10) * 6.28;
    float sway = sin(phase) * 4.0 ;
    vec3 swayOffset = vec3( windDir.x * sway, 0.0, windDir.y * sway);

    float randAngle = rand(float(gl_InstanceID)) * 6.2831853;
    vec3 toCamera = normalize(camPos - bladePos);
    vec3 bladeForward = normalize(vec3(instanceTransform[2]));
    bladeForward = mat3(cos(randAngle), 0, sin(randAngle), 0, 1, 0, -sin(randAngle), 0, cos(randAngle)) * bladeForward;
    vec3 newForward = normalize(mix(bladeForward, toCamera, 0.5));
    vec3 right = normalize(cross(vec3(0, 1, 0), newForward));
    vec3 up = cross(newForward, right);

    mat4 finalTransform = mat4(vec4(right * scale.x, 0),
                               vec4(up * scale.y + swayOffset, 0),
                               vec4(newForward * scale.z, 0),
                               vec4(bladePos, 1));

    vec4 worldPos = finalTransform * vec4(vertexPosition, 1.0);


    fragPosition = worldPos.xyz;
    fragTexCoord = vertexTexCoord;
    fragNormal = normalize(mat3(finalTransform) * vertexNormal);
    fragPosLightSpace = lightVP * worldPos;
    gl_Position = mvp * worldPos;
}
