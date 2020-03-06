#version 330 core
layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aNormal;

uniform float yPos;
uniform float height;
uniform float width;

out vec3 vNormal;

void main()
{
    vec3 oPos = aPos * .8;
    vNormal = aNormal;
    float x = oPos.x * cos(yPos) - oPos.z * sin(yPos);
    float z = oPos.z * cos(yPos) + oPos.x * sin(yPos);
    gl_Position = vec4(x/width*height, oPos.y, z, 1.0);
}