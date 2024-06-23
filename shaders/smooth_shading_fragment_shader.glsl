#version 460 core

// Output color
out vec4 FragColor;
in vec3 normal;
in vec3 color;
in float z;
void main()
{
    //float intensity =  (1.8 + z)/2.8 * (1.5 + normal.z)/2.5;
    float intensity =  (1.2 + normal.y)/2.2;
    FragColor = vec4(color, 1.0)*intensity;
}
