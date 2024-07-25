#version 460 core

uniform vec4 q, q_inv;
layout(location = 0) in vec3 in_pos;        // Vertex position
layout(location = 1) in vec3 in_color;        // Vertex colors

out vec3 v_color;

vec4 mul_quatern(vec4 u, vec4 v){
    //u.x, u.y, u.z, u.w = u
    //v.x, v.y, v.z, v.w = v
    return vec4(
            -v.y * u.y - v.z * u.z - v.w * u.w + v.x * u.x,
            v.y * u.x + v.z * u.w - v.w * u.z + v.x * u.y,
            -v.y * u.w + v.z * u.x + v.w * u.y + v.x * u.z,
            v.y * u.z - v.z * u.y + v.w * u.x + v.x * u.w);
}
vec3 rotation(vec3 vector){
    // quaternion mouse rotation
    vec4 qvector = vec4(0, vector);
    qvector = mul_quatern(qvector, q_inv);
    qvector = mul_quatern(q, qvector);
    return qvector.yzw;
}
void main()
{
    gl_Position = vec4(rotation(in_pos), 1.0);
    v_color = in_color;
}
