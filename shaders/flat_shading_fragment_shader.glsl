#version 460 core

// Output color
out vec4 FragColor;
in vec3 position; // flat shading
in vec3 color;
void main()
{

    vec3 x_tangent = dFdx(position);
    vec3 y_tangent = dFdy(position);
    vec3 face_normal = normalize( cross( x_tangent, y_tangent ) );

    float intensity =  (1.2 + face_normal.z)/2.2;
    FragColor = vec4(color, 1.0)*intensity;
}
