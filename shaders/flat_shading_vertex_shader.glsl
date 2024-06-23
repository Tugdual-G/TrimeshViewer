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

uniform float time;
layout(location = 0) in vec3 in_pos;        // Vertex position
layout(location = 1) in vec3 in_color;        // Vertex normal
out vec3 position;// flat shading
out vec3 color;
void main()
{
    // rotation
    float theta = 3.14*time;
    vec3 axis = vec3(1/sqrt(3.0),-1/sqrt(3.0), -1/sqrt(3.0));
    vec4 q = vec4(cos(theta), axis*sin(theta));
    vec4 q_c = vec4(q.x, -q.yzw);

    vec4 pos = mul_quatern(vec4(0.0, in_pos.xyz), q_c);
    pos = mul_quatern(q, pos);
    position = pos.yzw;// for flat shading
    gl_Position = vec4(pos.yzw, 1.0);
    color = in_color;

}
