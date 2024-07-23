#version 460 core

layout (lines_adjacency) in;
layout (triangle_strip, max_vertices = 24) out;
out vec3 normal;
out vec3 position;

uniform float zoom_level;
uniform vec2 viewport_size;

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

    float r = 0.01;

    vec3 circle[7]=vec3[7](vec3(0 , 1 , 0),
                            vec3(0 , 0.5 , 0.866025),
                            vec3(0 , -0.5 , 0.866025),
                            vec3(0 , -1 , 0.0),
                            vec3(0 , -0.5 , -0.866025),
                            vec3(0 , 0.5 , -0.866025),
                            vec3(0 , 1 , 0));

    vec3 normals[14];
    vec3 vertices[14];

    // start and end tangent
    vec3 T0 = normalize(gl_in[1].gl_Position.xyz - gl_in[0].gl_Position.xyz);
    vec3 T1 = normalize(gl_in[2].gl_Position.xyz - gl_in[1].gl_Position.xyz);
    vec3 T2 = normalize(gl_in[3].gl_Position.xyz - gl_in[2].gl_Position.xyz);

    vec3 N = normal_vec(T1);
    vec3 B = cross(T1, N);
    mat3 transform;
    transform[0] = T1;
    transform[1] = N * r;
    transform[2] = B * r;


    float l_T; // length to the meeting point

    for (int i = 0 ; i < 7 ; ++i){
        circle[i] = transform*circle[i];
        normals[i] = normalize(circle[i]);

        l_T = dot(circle[i], T1+T0)/dot(T1, T1+T0);

        vertices[i] = gl_in[1].gl_Position.xyz  + circle[i] - l_T * T1;
        vertices[i].xy *= -2/(vertices[i].z - 2); // perspective
        vertices[i].xy = vertices[i].xy * zoom_level;
        vertices[i].x *= viewport_size.y/viewport_size.x; //aspect ratio
    }

    for (int i = 7 ; i < 14 ; ++i){
        normals[i] = normalize(circle[i-7]);

        l_T = dot(circle[i-7], T1+T2)/dot(T1, T1+T2);

        vertices[i] = gl_in[2].gl_Position.xyz  + circle[i-7] - l_T * T1;
        vertices[i].xy *= -2/(vertices[i].z - 2); // perspective
        vertices[i].xy = vertices[i].xy * zoom_level;
        vertices[i].x *= viewport_size.y/viewport_size.x; //aspect ratio
    }


    for (int i = 0; i < 6; ++i) {
        gl_Position = vec4(vertices[i], 1);
        position = vertices[i];
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
        EndPrimitive();
    }

}

void main() {

    build_tube();

}
