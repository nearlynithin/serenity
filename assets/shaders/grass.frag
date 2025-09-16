#version 330

// Input vertex attributes (from vertex shader)
in vec3 fragPosition;
in vec2 fragTexCoord;
in vec3 fragNormal;
in vec4 fragPosLightSpace;
in vec4 fragColor;

// Input uniform values
uniform sampler2D texture0;
uniform vec4 colDiffuse;

// Output fragment color
out vec4 finalColor;

// Input lighting values - single directional light
uniform vec3 lightDir;
uniform vec4 lightColor;
uniform vec4 ambient;
uniform vec3 viewPos;

// Shadow mapping
uniform mat4 lightVP;
uniform sampler2D shadowMap;
uniform int shadowMapResolution;
uniform int renderPass; // 0 = shadow pass, 1 = main pass

uniform float shininess;
uniform float specularStrength;

// Fog parameters
uniform float fogDensity;
const vec4 fogColor = vec4(0.8, 1.0, 0.8, 1.0);

float calculateShadow() {
    // FIXED: Use correct variable name
    vec3 projCoords = fragPosLightSpace.xyz / fragPosLightSpace.w;
    projCoords = projCoords * 0.5 + 0.5;

    if (projCoords.x < 0.0 || projCoords.x > 1.0 ||
        projCoords.y < 0.0 || projCoords.y > 1.0 ||
        projCoords.z > 1.0)
        return 0.0;

    float curDepth = projCoords.z;

    vec3 normal = normalize(fragNormal);
    vec3 lightDirection = normalize(-lightDir);
    float cosTheta = clamp(dot(normal, lightDirection), 0.0, 1.0);
    float bias = max(0.01 * (1.0 - cosTheta), 0.001);
    bias += curDepth * 0.001;

    int shadowCounter = 0;
    int samples = 9;
    vec2 texelSize = 1.0 / vec2(shadowMapResolution);

    for (int x = -1; x <= 1; x++) {
        for (int y = -1; y <= 1; y++) {
            vec2 offset = texelSize * vec2(x, y);
            float sampleDepth = texture(shadowMap, projCoords.xy + offset).r;
            if (curDepth - bias > sampleDepth)
                shadowCounter++;
        }
    }

    float shadow = float(shadowCounter) / float(samples);
    shadow = smoothstep(0.0, 1.0, shadow);
    return shadow * 0.8;
}

void main() {
    // Shadow pass: simple alpha test
    if (renderPass == 0) {
        vec4 texelColor = texture(texture0, fragTexCoord);
        if (texelColor.a < 0.3) discard;
        return;
    }

    // Main pass
    vec4 texelColor = texture(texture0, fragTexCoord);
    if (texelColor.a < 0.3) discard;

    // Normalize inputs
    vec3 normal = normalize(fragNormal);
    vec3 viewDir = normalize(viewPos - fragPosition);
    vec3 lightDirection = normalize(-lightDir);

    // Regular diffuse lighting
    float diff = max(dot(normal, lightDirection), 0.0);
    vec3 diffuse = diff * lightColor.rgb;

    // Specular lighting (Blinn-Phong)
    vec3 halfwayDir = normalize(lightDirection + viewDir);
    float spec = pow(max(dot(normal, halfwayDir), 0.0), shininess * 0.3);
    vec3 specular = spec * lightColor.rgb * specularStrength * 0.2;

    // === NEW: RIM LIGHTING / FRESNEL EFFECT ===
    float rimPower = 2.0;
    float rimIntensity = 0.8;
    float rim = 1.0 - max(dot(viewDir, normal), 0.0);
    rim = pow(rim, rimPower);
    vec3 rimLighting = rim * lightColor.rgb * rimIntensity;

    // === NEW: TRANSLUCENCY / SUBSURFACE SCATTERING ===
    // When light hits grass from behind, it should glow
    float translucency = 0.6;
    float backLightDot = max(dot(-viewDir, lightDirection), 0.0);
    float backLight = pow(backLightDot, 4.0) * translucency;
    vec3 translucencyColor = backLight * lightColor.rgb * texelColor.rgb;

    // === NEW: ENHANCED BACKFACE LIGHTING ===
    // Add extra lighting when facing the light source
    float facingLight = max(dot(-viewDir, lightDirection), 0.0);
    vec3 backfaceLighting = facingLight * lightColor.rgb * 0.3;

    // Calculate shadow
    float shadow = calculateShadow();

    // Apply lighting with shadow (but don't shadow rim lighting completely)
    vec3 lighting = (diffuse + specular) * (1.0 - shadow);

    // Add rim lighting (partially affected by shadow)
    lighting += rimLighting * (1.0 - shadow * 0.7); // Rim light less affected by shadows

    // Add translucency (not affected by shadows - light passes through)
    lighting += translucencyColor;

    // Add backface lighting for when facing light
    lighting += backfaceLighting * (1.0 - shadow * 0.5);

    // Enhanced ambient for grass
    vec3 ambientUp = vec3(0.0, 1.0, 0.0);
    float skyFactor = dot(normal, ambientUp) * 0.5 + 0.5;
    vec3 enhancedAmbient = ambient.rgb * (0.4 + 0.5 * skyFactor);

    vec3 finalLighting = lighting + enhancedAmbient;

    // Ensure grass doesn't go completely dark
    finalLighting = max(finalLighting, ambient.rgb * 0.3);

    finalColor = texelColor * colDiffuse * vec4(finalLighting, 1.0);

    // Tone mapping
    finalColor.rgb = finalColor.rgb / (finalColor.rgb + vec3(1.0));

    // Fog calculation
    float dist = length(viewPos - fragPosition);
    float heightFactor = clamp((fragPosition.y + 10.0) / 20.0, 0.1, 1.0);
    dist *= 0.02 * heightFactor;

    float fogFactor = 1.0/exp((dist*fogDensity)*(dist*fogDensity));
    fogFactor = clamp(fogFactor, 0.0, 1.0);

    // Apply fog
    finalColor = mix(fogColor, finalColor, fogFactor);
    // finalColor = fragColor;
}
