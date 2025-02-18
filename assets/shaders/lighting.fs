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

// Input lighting values
uniform vec3 lightDir;
uniform vec4 lightColor;
uniform vec4 ambient;
uniform vec3 viewPos;

// Fog parameters
uniform float fogDensity;
const vec4 fogColor = vec4(0.8, 1.0, 0.8, 1.0);

void main()
{
    // Texel color fetching from texture sampler
    vec4 texelColor = texture(texture0, fragTexCoord);
    
    // Calculate lighting
    vec3 lightDot = vec3(0.0);
    vec3 normal = normalize(fragNormal);
    vec3 viewD = normalize(viewPos - fragPosition);
    vec3 specular = vec3(0.0);
    
    vec3 l = -lightDir;
    float NdotL = max(dot(normal, l), 0.0);
    lightDot += lightColor.rgb * NdotL;
    
    float specCo = 0.0;
    if (NdotL > 0.0) {
        specCo = pow(max(0.0, dot(viewD, reflect(-(l), normal))), 16.0); // 16 refers to shine
    }
    specular += specCo;
    
    finalColor = (texelColor*((colDiffuse + vec4(specular, 1.0))*vec4(lightDot, 1.0)));
    finalColor += texelColor*(ambient/10.0)*colDiffuse;
    
    // Gamma correction
    finalColor = pow(finalColor, vec4(1.0/2.2));
    
    // Fog calculation
    float dist = length(viewPos - fragPosition);
    dist *= 0.02;
    float fogFactor = 1.0/exp((dist*fogDensity)*(dist*fogDensity));
    fogFactor = clamp(fogFactor, 0.0, 1.0);
    
    // Apply fog
    finalColor = mix(fogColor, finalColor, fogFactor);
}