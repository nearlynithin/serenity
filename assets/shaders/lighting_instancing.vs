#version 330

// Input vertex attributes
in vec3 vertexPosition;
in vec2 vertexTexCoord;
in vec3 vertexNormal;
//in vec4 vertexColor;      // Not required

in mat4 instanceTransform;

// Input uniform values
uniform mat4 mvp;
uniform mat4 matNormal;
uniform float time;
uniform vec3 cameraPos;
uniform vec3 cameraTarget;

// Output vertex attributes (to fragment shader)
out vec3 fragPosition;
out vec2 fragTexCoord;
out vec4 fragColor;
out vec3 fragNormal;

const float PI = 3.14159265358979323846f;

// NOTE: Add here your custom variables

void main()
{

    float scaleX = length(vec3(instanceTransform[0]));
    float scaleY = length(vec3(instanceTransform[1]));
    float scaleZ = length(vec3(instanceTransform[2]));


    float bend = vertexPosition.y * 20.0;
    float sway_offset = float(gl_InstanceID) * 0.5;
    float sway = sin(time +  sway_offset) * 0.01 * bend;

    vec3 displaced = vertexPosition + vec3(0.0,0.0,sway);

    vec3 bladePos = vec3(instanceTransform[3]); // translation from matrix
    vec3 toCamera = normalize(cameraPos - bladePos);
        // Get the original forward direction of the blade (Z axis of instance)
    vec3 bladeForward = normalize(vec3(instanceTransform[2]));

    // Compute a rotation blend factor (e.g., only some blades face the camera)
    float faceFactor = 0.5; // 0.0 = no face, 1.0 = full billboard
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

    // Send vertex attributes to fragment shader
    fragPosition = vec3(facingMat*vec4(displaced, 1.0));
    fragTexCoord = vertexTexCoord;
    //fragColor = vertexColor;
    fragNormal = normalize(vec3(matNormal*vec4(vertexNormal, 1.0)));

    // Calculate final vertex position, note that we multiply mvp by instanceTransform
    gl_Position = mvp*facingMat*vec4(displaced, 1.0);
}
