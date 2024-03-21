#version 450

layout(binding = 0) uniform UniformBufferObject {
    mat4 model;
    mat4 view;
    mat4 proj;
} ubo;

layout(location = 0) in vec3 inPosition;
layout(location = 1) in vec3 inNormal;
layout(location = 2) in vec2 inTexCoord;

layout(location = 0) out float lightIntensity;
layout(location = 1) out vec2 fragTexCoord;

vec3 lightPosition = vec3(40,40,40);


void main() {
    gl_Position = ubo.proj * ubo.view * ubo.model * vec4(inPosition, 1.0);

    // Calculate light direction
    vec3 lightDirection = normalize(vec3(gl_Position.xyz) - inPosition);

    // Ensure the normal is normalized
    vec3 normal = normalize(inNormal);

    // Calculate the Lambertian diffuse reflection (dot product of normal and light direction)
    lightIntensity = clamp(0.4 + max(dot(normal, lightDirection), 0.0), 0, 1);

    fragTexCoord = inTexCoord;
}