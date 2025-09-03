#version 330

// Input vertex attributes
in vec3 vertexPosition;
in vec2 vertexTexCoord;
in vec3 vertexNormal;
in mat4 instanceTransform;

// Input uniform values
uniform mat4 mvp;
uniform mat4 matNormal;
uniform float time;
uniform vec3 camPos;
uniform vec3 camTarget;

// Output vertex attributes (to fragment shader)
out vec3 fragPosition;
out vec2 fragTexCoord;
out vec4 fragColor;
out vec3 fragNormal;

const float PI = 3.14159265358979323846f;

void main()
{
    float scaleX = length(vec3(instanceTransform[0]));
    float scaleY = length(vec3(instanceTransform[1]));
    float scaleZ = length(vec3(instanceTransform[2]));

    float bend = vertexPosition.y * 30.0;
    float sway_offset = float(gl_InstanceID) * 0.9;
    float sway = sin(time + sway_offset) * 0.01 * bend;
    vec3 displaced = vertexPosition + vec3(0.0, 0.0, sway);

    vec3 bladePos = vec3(instanceTransform[3]); // translation from matrix
    vec3 toCamera = normalize(camPos - bladePos);

    // Get the original forward direction of the blade (Z axis of instance)
    vec3 bladeForward = normalize(vec3(instanceTransform[2]));

    // Compute a rotation blend factor (e.g., only some blades face the camera)
    float faceFactor = 0.5;

    // Interpolate forward direction between original and camera-facing
    vec3 newForward = normalize(mix(bladeForward, toCamera, faceFactor));

    // Compute new right vector (cross up and forward)
    vec3 up = vec3(0.0, 1.0, 0.0);
    vec3 right = normalize(cross(up, newForward));
    up = cross(newForward, right); // recalculate orthogonal up

    // Build new orientation matrix
    mat4 facingMat = mat4(
        vec4(right * scaleX, 0.0),
        vec4(up * scaleY, 0.0),
        vec4(newForward * scaleZ, 0.0),
        vec4(bladePos, 1.0)
    );

    // FIXED: Use the same transformation for fragPosition as for gl_Position
    vec4 worldPos = facingMat * vec4(displaced, 1.0);
    fragPosition = worldPos.xyz;

    fragTexCoord = vertexTexCoord;

    // FIXED: Transform normal using the same facing matrix
    // Extract rotation part of facingMat for normal transformation
    mat3 normalMatrix = mat3(
        normalize(right),
        normalize(up),
        normalize(newForward)
    );
    fragNormal = normalize(normalMatrix * vertexNormal);

    // Calculate final vertex position
    gl_Position = mvp * worldPos;
}
