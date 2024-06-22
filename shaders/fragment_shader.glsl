#version 460 core

// Output color
out vec4 FragColor;
in vec3 normal;
in vec3 position; // flat shading
void main()
{

    vec3 x_tangent = dFdx(position);
    vec3 y_tangent = dFdy(position);
    vec3 face_normal = normalize( cross( x_tangent, y_tangent ) );

    float intensity =  (1.5 + face_normal.z)/2.0;
    FragColor = vec4(0.01, 0.8, 0.7, 1.0)*intensity;
}
