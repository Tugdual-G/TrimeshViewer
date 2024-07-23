#version 460 core

in vec3 normal;
in vec3 position;
// Output color
out vec4 FragColor;
vec3 light_position = vec3(1,1,2);
float ambient_light = 0.8;
void main()
{
    vec3 light_direction = normalize(light_position);
    float diffusion = dot(light_direction, normal);

    float intensity = (ambient_light + diffusion)/(1.0 + ambient_light);
    FragColor = vec4(vec3(1.0, (1.0 + position.z)/2.0, 1.0 -(1.0 + position.z)/2.0)*intensity,0.3);
}
