#version 330

// Input vertex attributes (from vertex shader)
in vec3 fragPosition;
in vec2 fragTexCoord;
in vec3 fragNormal;

// Input uniform values
uniform sampler2D texture0;
uniform vec4 colDiffuse;

// Output fragment color
out vec4 finalColor;

#define MAX_LIGHTS 5
#define LIGHT_DIRECTIONAL 0
#define LIGHT_POINT 1

struct Light {
    int enabled;
    int type;
    vec3 position;
    vec3 target;
    vec4 color;
    float intensity;
    float attenuation;
};

uniform Light lights[MAX_LIGHTS];

// Input lighting values
uniform vec3 lightDir;
uniform vec4 lightColor;
uniform vec4 ambient;
uniform vec3 viewPos;

uniform float shininess;
uniform float specularStrength;
uniform float metallicFactor;

// Fog parameters
uniform float fogDensity;
// const vec4 fogColor = vec4(0.8, 1.0, 0.8, 1.0);
const vec4 fogColor = vec4(0.18,0.14,0.16, 1.0);

vec3 calculateDirectionalLight(Light light, vec3 normal, vec3 viewDir) {
    vec3 lightDir = normalize(light.position - light.target);

    // Diffuse
    float diff = max(dot(normal, lightDir), 0.0);
    vec3 diffuse = diff * light.color.rgb * light.intensity;

    // Specular (Blinn-Phong)
    vec3 halfwayDir = normalize(lightDir + viewDir);
    float spec = pow(max(dot(normal, halfwayDir), 0.0), shininess);
    vec3 specular = spec * light.color.rgb * specularStrength;

    return diffuse + specular;
}

vec3 calculatePointLight(Light light, vec3 fragPos, vec3 normal, vec3 viewDir) {
    vec3 lightDir = normalize(light.position - fragPos);
    float distance = length(light.position - fragPos);

    // Attenuation (quadratic falloff)
    float attenuation = 1.0 / (1.0 + light.attenuation * distance +
                              light.attenuation * 0.1 * distance * distance);

    // Diffuse
    float diff = max(dot(normal, lightDir), 0.0);
    vec3 diffuse = diff * light.color.rgb * light.intensity;

    // Specular (Blinn-Phong)
    vec3 halfwayDir = normalize(lightDir + viewDir);
    float spec = pow(max(dot(normal, halfwayDir), 0.0), shininess);
    vec3 specular = spec * light.color.rgb * specularStrength;

    return (diffuse + specular) * attenuation;
}

// Fresnel effect for more realistic reflections
float fresnel(vec3 viewDir, vec3 normal, float f0) {
    float cosTheta = max(dot(viewDir, normal), 0.0);
    return f0 + (1.0 - f0) * pow(1.0 - cosTheta, 5.0);
}

void main() {
    vec4 texelColor = texture(texture0, fragTexCoord);
    if (texelColor.a < 0.3) discard;

    // Normalize inputs
    vec3 normal = normalize(fragNormal);
    vec3 viewDir = normalize(viewPos - fragPosition);

    vec3 totalLighting = vec3(0.0);

    for (int i = 0; i < MAX_LIGHTS; i++) {
        if (lights[i].enabled == 1) {
            if (lights[i].type == LIGHT_DIRECTIONAL) {
                totalLighting += calculateDirectionalLight(lights[i], normal, viewDir);
            } else if (lights[i].type == LIGHT_POINT) {
                totalLighting += calculatePointLight(lights[i], fragPosition, normal, viewDir);
            }
        }
    }

    vec3 mainLightDir = normalize(-lightDir);
    float mainDiff = max(dot(normal, mainLightDir), 0.0);
    vec3 mainDiffuse = mainDiff * lightColor.rgb;

    vec3 mainHalfway = normalize(mainLightDir + viewDir);
    float mainSpec = pow(max(dot(normal, mainHalfway), 0.0), shininess);
    vec3 mainSpecular = mainSpec * lightColor.rgb * specularStrength;

    totalLighting += mainDiffuse + mainSpecular;

    // Apply Fresnel effect for more realistic metallic surfaces
    float fresnelFactor = fresnel(viewDir, normal, 0.04);
    vec3 fresnelColor = mix(vec3(1.0), texelColor.rgb, metallicFactor);
    totalLighting = mix(totalLighting, totalLighting * fresnelColor, fresnelFactor * metallicFactor);

    // Darker ambient lighting with slight directional bias
    vec3 ambientUp = vec3(0.0, 1.0, 0.0);
    float skyFactor = dot(normal, ambientUp) * 0.5 + 0.5;
    vec3 enhancedAmbient = ambient.rgb * (0.1 + 0.2 * skyFactor);

    vec3 finalLighting = totalLighting + enhancedAmbient;

    finalColor = texelColor * colDiffuse * vec4(finalLighting, 1.0);

    finalColor.rgb = finalColor.rgb / (finalColor.rgb + vec3(1.0));

    finalColor = pow(finalColor, vec4(1.0/2.2));

    float dist = length(viewPos - fragPosition);
    float heightFactor = clamp((fragPosition.y + 10.0) / 20.0, 0.1, 1.0);
    dist *= 0.05 * heightFactor;

    float fogFactor = 1.0/exp((dist*fogDensity)*(dist*fogDensity));
    fogFactor = clamp(fogFactor, 0.0, 1.0);

    // Apply fog
    finalColor = mix(fogColor, finalColor, fogFactor);
}
