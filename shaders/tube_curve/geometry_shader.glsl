#version 460 core

layout (lines_adjacency) in;
layout (triangle_strip, max_vertices = 36) out;
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
    if (abs(T.x) > 0.0001 || abs(T.y) > 0.0001){
        normal.x = -T.y;
        normal.y = T.x;
        normal = normalize(normal);
    }else{
        normal.x = 1;
    }

    return normal;
}

void build_tube(){

    float width = 0.002;

    vec3 vertices[16]=vec3[16](vec3(0 , 1 , 0),
                            vec3(0 , 0.5 , 0.866025),
                            vec3(0 , -0.5 , 0.866025),
                            vec3(0 , -1 , 0.0),
                            vec3(0 , -0.5 , -0.866025),
                            vec3(0 , 0.5 , -0.866025),
                            vec3(0 , 1 , 0),
                            vec3(1 , 1 , 0),
                            vec3(1 , 0.5 , 0.866025),
                            vec3(1 , -0.5 , 0.866025),
                            vec3(1 , -1 , 0.0),
                            vec3(1 , -0.5 , -0.866025),
                            vec3(1 , 0.5 , -0.866025),
                            vec3(1 , 1 , 0),
                            vec3(0 , 0 , 0),
                            vec3(1 , 0 , 0));

    vec3 normals[16];

    // start and end tangent
    vec3 T = gl_in[2].gl_Position.xyz - gl_in[1].gl_Position.xyz;
    float len = length(T);
    T = T/len;
    vertices[14].x -= width/len;
    vertices[15].x += width/len;


    vec3 N = normal_vec(T);
    vec3 B = cross(T, N);
    mat3 transform;
    transform[0] = len*T;
    transform[1] = N*width;
    transform[2] = B*width;
    for (int i = 0 ; i < 14 ; ++i){
        normals[i] = transform*vec3(0,vertices[i].yz);
        normals[i] = normalize(normals[i]);
        }
    normals[14] = vec3(-1, 0,0);
    normals[15] = vec3(1, 0,0);

    for (int i = 0 ; i < 16 ; ++i){
        vertices[i] = gl_in[1].gl_Position.xyz  + transform*vertices[i];


        vec4 qnormals = vec4(0,normals[i]);
        qnormals = mul_quatern(qnormals, q_inv);
        qnormals = mul_quatern(q, qnormals);
        normals[i] = qnormals.yzw;

        vec4 qpos = vec4(0,vertices[i].xyz);
        qpos = mul_quatern(qpos, q_inv);
        qpos = mul_quatern(q, qpos);


        qpos.yz *= -2/(qpos.w - 2); // perspective
        qpos.yz = qpos.yz * zoom_level;
        qpos.y *= viewport_size.y/viewport_size.x; //aspect ratio
        vertices[i] = qpos.yzw;
    }


    for (int i = 0; i < 6; ++i) {
        gl_Position = vec4(vertices[14], 1);
        normal = normals[i];
        position = vertices[14];
        EmitVertex();

        gl_Position = vec4(vertices[i], 1);
        normal = normals[i];
        EmitVertex();

        gl_Position = vec4(vertices[i + 1], 1);
        normal = normals[i + 1];
        EmitVertex();

        gl_Position = vec4(vertices[i + 7], 1);
        normal = normals[i + 7];
        EmitVertex();

        gl_Position =  vec4(vertices[i + 8], 1);
        normal =  normals[i + 8];
        EmitVertex();

        gl_Position = vec4(vertices[15], 1);
        normal = normals[i + 8];
        EmitVertex();
        EndPrimitive();
    }

}

void main() {

    build_tube();

}
