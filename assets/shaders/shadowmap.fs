#version 330

// Input vertex attributes (from vertex shader)
in vec3 fragPosition;
in vec2 fragTexCoord;
in vec4 fragColor;
in vec3 fragNormal;

// Output fragment color
out vec4 finalColor;

void main()
{
    // For shadow map rendering, we only need to write depth
    // The depth buffer automatically stores the depth values
    // We can output a simple color or discard fragments if needed
    finalColor = vec4(1.0);
}
