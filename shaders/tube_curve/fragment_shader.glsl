#version 460 core

in vec3 normal;
in vec3 position;
// Output color
out vec4 FragColor;
float ambient_light = 0.8;
float specular_light_strength = 0.5;
vec3 light_position = vec3(2,2,2);
void main()
{

    vec3 light_direction = normalize(light_position);
    float diffusion = dot(light_direction, normal);

    vec3 fragment_view_direction = normalize(vec3(0,0,1)-position);
    vec3 reflected_light_direction = 2*dot(normal, light_direction) * normal-light_direction;
    float specular_ligth = pow(max(dot(reflected_light_direction, fragment_view_direction), 0.0), 16);
    specular_ligth *= specular_light_strength;

    float intensity =  (specular_ligth + ambient_light + diffusion)/(1.0 + ambient_light);

    FragColor = vec4(vec3(1.0, (1.0 + position.z)/2.0, 1.0 -(1.0 + position.z)/2.0)*intensity,1.0);
}
