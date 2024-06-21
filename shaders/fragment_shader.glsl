#version 460 core


// Output color
out vec4 FragColor;
in vec3 normal;
void main()
{
    float intensity =  (1+normal.y)/2.0;
    FragColor = vec4(0.01, 0.8, 1.0, 1.0)*intensity;
}
