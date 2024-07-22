#version 460 core

vec4 mul_quatern(vec4 u, vec4 v){
    //u.x, u.y, u.z, u.w = u
    //v.x, v.y, v.z, v.w = v
    return vec4(
            -v.y * u.y - v.z * u.z - v.w * u.w + v.x * u.x,
            v.y * u.x + v.z * u.w - v.w * u.z + v.x * u.y,
            -v.y * u.w + v.z * u.x + v.w * u.y + v.x * u.z,
            v.y * u.z - v.z * u.y + v.w * u.x + v.x * u.w);
}

uniform vec4 q, q_inv;
uniform float zoom_level;
uniform vec2 viewport_size;
layout(location = 0) in vec3 in_pos;        // Vertex position
layout(location = 1) in vec3 in_color;        // Vertex colors
layout(location = 2) in vec3 vector_base_coord;
layout(location = 3) in vec3 vector_direction;
layout(location = 4) in vec3 vector_color;
out vec3 position;// flat shading
out vec3 color;
void main()
{


    // Transforms the vector base instance into the desired one
    mat3 transform;
    vec3 e_i = normalize(vector_direction);
    vec3 e_k = normalize(cross(vec3(1,0,0), e_i));
    vec3 e_j = cross(e_k, e_i);
    transform[0] = e_i*length(vector_direction);
    transform[1] = e_j*length(vector_direction);
    transform[2] = e_k*length(vector_direction);
    vec4 pos = vec4(0,vector_base_coord + transform*in_pos);

    pos = mul_quatern(pos, q_inv);
    pos = mul_quatern(q, pos);
    position = pos.yzw;// for flat shading
    pos.yz *= -2/(pos.w - 2); // perspective
    pos.yz = pos.yz * zoom_level;
    pos.y *= viewport_size.y/viewport_size.x; //aspect ratio
    gl_Position = vec4(pos.yzw, 1.0);
    color = vector_color;

}
