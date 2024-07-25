#version 460 core

in vec3 normal;
in vec3 position;
in vec3 color;
// Output color
out vec4 FragColor;
vec3 light_position = vec3(1,1,2);
float ambient_light = 0.9;
void main()
{
    vec3 light_direction = normalize(light_position);
    float diffusion = dot(light_direction, normal);

    float intensity = (ambient_light + diffusion)/(1.0 + ambient_light);

    FragColor = vec4(color*intensity,0.5);
    // FragColor = vec4(vec3(1.0, 0.3, 1.0)*intensity,0.8);
}
