#version 450

layout(location = 0) in float lightIntensity;
layout(location = 1) in vec2 fragTexCoord;

layout(location = 0) out vec4 outColor;

void main() {
    outColor = vec4(lightIntensity,lightIntensity,lightIntensity, 1.0);
    outColor.rgb = pow(outColor.rgb, vec3(1.0/2.2));
}