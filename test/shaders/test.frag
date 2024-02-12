#version 430 core

layout(location=0) in vec2 inUV;

layout(location=0) out vec4 outFragColor;

uniform sampler2D uTexture;

void main() {
    outFragColor = texture(uTexture, inUV);
}