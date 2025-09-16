#version 330

in vec3 fragPosition;
in vec2 fragTexCoord;
in vec3 fragNormal;
uniform sampler2D texture0;
uniform vec4 colDiffuse;
out vec4 finalColor;
uniform vec3 lightDir;
uniform vec4 lightColor;
uniform vec4 ambient;
uniform vec3 viewPos;
uniform mat4 lightVP;
uniform sampler2D shadowMap;
uniform int shadowMapResolution;

void main()
{
    vec4 texelColor = texture(texture0, fragTexCoord);
    vec3 normal = normalize(fragNormal);
    vec3 viewD = normalize(viewPos - fragPosition);
    vec3 l = normalize(-lightDir);
    float NdotL = max(dot(normal, l), 0.0);

    // Reduced diffuse intensity
    vec3 diffuse = lightColor.rgb * NdotL * 0.1; // Reduced from 1.0 to 0.7

    float spec = 0.0;
    if (NdotL > 0.0)
        spec = pow(max(dot(viewD, reflect(-l, normal)), 0.0), 32.0);

    // Reduced specular intensity
    vec3 specular = lightColor.rgb * spec * 0.8; // Reduced from 0.4 to 0.2

    // Shadow mapping with softer filtering
    vec4 fragPosLightSpace = lightVP * vec4(fragPosition, 1.0);
    fragPosLightSpace.xyz /= fragPosLightSpace.w;
    fragPosLightSpace.xyz = fragPosLightSpace.xyz * 0.5 + 0.5;
    vec2 sampleCoords = fragPosLightSpace.xy;
    float curDepth = fragPosLightSpace.z;
    float closestDepth = texture(shadowMap, fragPosLightSpace.xy).r;
    float bias = 0.001;

    int shadowCounter = 0;
    const int numSamples = 20; // Increased from 9 to 25 for softer shadows
    vec2 texelSize = 1.5 / vec2(shadowMapResolution); // Larger sample area for softer edges

    // 5x5 sampling pattern for softer shadows
    for (int x = -2; x <= 2; x++)
    {
        for (int y = -2; y <= 2; y++)
        {
            float sampleDepth = texture(shadowMap, sampleCoords + texelSize * vec2(x, y)).r;
            if (curDepth - bias > closestDepth)
                shadowCounter++;
        }
    }

    float shadow = float(shadowCounter) / float(numSamples);

    // Soften shadow transition - less harsh contrast
    shadow = smoothstep(0.0, 1.0, shadow); // Smoother shadow falloff

    // Increased ambient for better balance and reduced overall brightness
    vec3 lighting = ambient.rgb * 0.2 + (1.0 - shadow) * (diffuse + specular);

    finalColor = vec4(texelColor.rgb * lighting, texelColor.a);

    // Slightly stronger gamma correction for less brightness
    finalColor.rgb = pow(finalColor.rgb, vec3(1.0/2.0)); // Changed from 2.2 to 2.0

    // show normals
    // vec3 n = normalize(fragNormal);
    // vec3 color = n * 0.5 + 0.5;
    // finalColor = vec4(color, 1.0);
}
