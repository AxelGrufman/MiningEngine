#version 330 core

layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 acol;

out vec3 vertexColor;

uniform vec2 uOffset;
uniform vec2 uScale;
uniform float uRotation;

void main()
{
    vec2 scaledPos = aPos.xy * uScale;
    
    float c = cos(uRotation);
    float s = sin(uRotation);
    
    vec2 rotatedPos = vec2(scaledPos.x * c - scaledPos.y * s, scaledPos.x * s + scaledPos.y * c);
    
    gl_Position = vec4(rotatedPos + uOffset, aPos.z, 1.0);
    vertexColor = acol;
}
