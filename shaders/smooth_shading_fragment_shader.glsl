#version 460 core

// Output color
out vec4 FragColor;
in vec3 normal;
in vec3 color;
void main()
{
    float intensity =  (1.5 + normal.z)/2.5;
    FragColor = vec4(color, 1.0)*intensity;
}
