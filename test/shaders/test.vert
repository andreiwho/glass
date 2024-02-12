#version 430 core

layout(location=0) in vec3 inPosition;
layout(location=1) in vec2 inUV;

layout(location=0) out vec2 outUV;

uniform mat4 uViewProjection;

void main(void) {
    gl_Position = uViewProjection * vec4(inPosition, 1.0);
    outUV = inUV;
}