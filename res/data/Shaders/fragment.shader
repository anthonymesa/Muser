#version 330 core
out vec4 FragColor;

in vec3 vNormal;

void main()
{
    float brightness = max(dot(vNormal, normalize(vec3(.3, .4, -1))), .2);
    FragColor = vec4(vec3(1, 0, 0) * brightness, 1.0);
}