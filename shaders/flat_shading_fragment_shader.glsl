#version 460 core

// Output color
out vec4 FragColor;
in vec3 position; // flat shading
in vec3 color;
float ambient_light = 1.2;
float specular_light_strength = 0.4;
vec3 light_position = vec3(2,2,2);
void main()
{

    vec3 x_tangent = dFdx(position);
    vec3 y_tangent = dFdy(position);
    vec3 face_normal = normalize( cross( x_tangent, y_tangent ) );

    vec3 light_direction = normalize(light_position);
    float diffusion = dot(light_direction, face_normal);

    vec3 fragment_view_direction = normalize(vec3(0,0,1)-position);
    vec3 reflected_light_direction = 2*dot(face_normal, light_direction)*face_normal-light_direction;
    float specular_ligth = pow(max(dot(reflected_light_direction, fragment_view_direction), 0.0), 16);
    specular_ligth *= specular_light_strength;

    float intensity =  (specular_ligth + ambient_light + diffusion)/(1.0 + ambient_light);
    FragColor = vec4(color, 1.0)*intensity;
}
