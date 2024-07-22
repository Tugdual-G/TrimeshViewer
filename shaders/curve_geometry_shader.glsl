#version 460 core

layout (lines_adjacency) in;
layout (triangle_strip, max_vertices = 4) out;
out vec3 normal;
out vec3 position;

uniform vec4 q, q_inv;
uniform float zoom_level;
uniform vec2 viewport_size;

vec4 mul_quatern(vec4 u, vec4 v){
    //u.x, u.y, u.z, u.w = u
    //v.x, v.y, v.z, v.w = v
    return vec4(
            -v.y * u.y - v.z * u.z - v.w * u.w + v.x * u.x,
            v.y * u.x + v.z * u.w - v.w * u.z + v.x * u.y,
            -v.y * u.w + v.z * u.x + v.w * u.y + v.x * u.z,
            v.y * u.z - v.z * u.y + v.w * u.x + v.x * u.w);
}


vec3 normal_vec(vec3 T){
    // Normal vector to the tangent T.
    vec3 normal = vec3(0,0,0);
    if (abs(T.x) > 0.0 || abs(T.y) > 0.00){
        normal.x = -T.y;
        normal.y = T.x;
        normal = normalize(normal);
    }else{
        normal.x = 1;
    }

    return normal;
}

void build_quad_line(vec3 pos1, vec3 pos2){


    // quaternion mouse rotation
    vec4 qpos1 = vec4(0, pos1);
    qpos1 = mul_quatern(qpos1, q_inv);
    qpos1 = mul_quatern(q, qpos1);
    pos1 = qpos1.yzw;

    vec4 qpos2 = vec4(0, pos2);
    qpos2 = mul_quatern(qpos2, q_inv);
    qpos2 = mul_quatern(q, qpos2);
    pos2 = qpos2.yzw;

    // start and end tangent
    vec3 T = pos2 - pos1;

    float width = 0.002;
    // if (length(T.xy)<width){

    //     pos2.xy += normalize(T.xy)*width;

    // }
    float len = length(T);
    T = T/len;


    vec3 N = normal_vec(T);

    vec3 normals[4];
    vec3 vertices[4];

    vertices[0] = pos1 - width*N;
    normals[0] = normalize(-N+vec3(0,0,0.01));
    vertices[1] = pos1 + width*N;
    normals[1] = normalize(N+vec3(0,0,0.01));

    vertices[2] = pos2 - width*N;
    normals[2] = normalize(-N+vec3(0,0,0.01));
    vertices[3] = pos2 + width*N;
    normals[3] = normalize(N+vec3(0,0,0.01));

    for (int i = 0 ; i < 4 ; ++i){
        position = vertices[i];

        vec4 qpos = vec4(0,vertices[i]);
        qpos.yz *= -2/(qpos.w - 2); // perspective
        qpos.yz = qpos.yz * zoom_level;
        qpos.y *= viewport_size.y/viewport_size.x; //aspect ratio
        gl_Position = vec4(qpos.yzw, 1);
        normal = normals[i];
        EmitVertex();
    }
    EndPrimitive();

}

void main() {

    build_quad_line(gl_in[1].gl_Position.xyz, gl_in[2].gl_Position.xyz);

}
