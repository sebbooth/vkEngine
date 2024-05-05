#version 450

layout(binding = 1) uniform sampler2D texSampler;

layout(location = 0) in float lightIntensity;
layout(location = 1) in vec2 fragTexCoord;

layout(location = 0) out vec4 outColor;

void main() {
    outColor = lightIntensity * texture(texSampler, fragTexCoord);
    outColor.rgb = pow(outColor.rgb, vec3(1.0/2.2));

    outColor = texture(texSampler, fragTexCoord);
}