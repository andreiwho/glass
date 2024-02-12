#version 430 core

layout(location=0) in vec3 inColor;

layout(location=0) out vec4 outFragColor;

void main() {
    outFragColor = vec4(inColor.xyz, 1.0);
}