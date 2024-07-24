#version 460 core

layout (lines_adjacency) in;
layout (triangle_strip, max_vertices = 32) out;
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

    int n = 9;
    vec3 circle[8]=vec3[8](vec3(0, 1.0, 0.0),
                            vec3(0, 0.70711, 0.70711),
                            vec3(0, 0.0, 1.0),
                            vec3(0, -0.70711, 0.70711),
                            vec3(0, -1.0, 0.0),
                            vec3(0, -0.70711, -0.70711),
                            vec3(0, 0.0, -1.0),
                            vec3(0, 0.70711, -0.70711));

    // vec3 circle[7]=vec3[7](vec3(0 , 1 , 0),
    //                         vec3(0 , 0.5 , 0.866025),
    //                         vec3(0 , -0.5 , 0.866025),
    //                         vec3(0 , -1 , 0.0),
    //                         vec3(0 , -0.5 , -0.866025),
    //                         vec3(0 , 0.5 , -0.866025),
    //                         vec3(0 , 1 , 0));

    vec3 normals[18];
    vec3 vertices[18];

    // start and end tangent
    vec3 T0 = normalize(gl_in[1].gl_Position.xyz - gl_in[0].gl_Position.xyz);
    vec3 T1 = normalize(gl_in[2].gl_Position.xyz - gl_in[1].gl_Position.xyz);
    vec3 T2 = normalize(gl_in[3].gl_Position.xyz - gl_in[2].gl_Position.xyz);

    // vec3 N = normal_vec(T1);
    // vec3 B = cross(T1, N);
    // mat3 transform;
    // transform[0] = T1;
    // transform[1] = N ;
    // transform[2] = B ;


    vec3 N = normalize(cross(T0, T1));
    vec3 B = cross(T1, N);
    mat3 transform;
    transform[0] = T1;
    transform[1] = N ;
    transform[2] = B ;

    float l_T; // length to the meeting point

    for (int i = 0 ; i < n ; ++i){
        vertices[i] = transform*circle[i%(n-1)];
        normals[i] = normalize(vertices[i]);

        l_T = dot(vertices[i], T1+T0)/dot(T1, T1+T0);

        vertices[i] = gl_in[1].gl_Position.xyz  + r*(vertices[i] - l_T * T1);
        vertices[i].xy *= -2/(vertices[i].z - 2); // perspective
        vertices[i].xy = vertices[i].xy * zoom_level;
        vertices[i].x *= viewport_size.y/viewport_size.x; //aspect ratio
    }

    N = normalize(cross(T1, T2));
    B = cross(T1, N);
    transform;
    transform[0] = T1;
    transform[1] = N ;
    transform[2] = B ;


    for (int i = n ; i < 2*n ; ++i){
        vertices[i] = transform*circle[(i-n)%(n-1)];
        normals[i] = normalize(vertices[i]);

        l_T = dot(vertices[i], T1+T2)/dot(T1, T1+T2);

        vertices[i] = gl_in[2].gl_Position.xyz  + r*(vertices[i] - l_T * T1);
        vertices[i].xy *= -2/(vertices[i].z - 2); // perspective
        vertices[i].xy = vertices[i].xy * zoom_level;
        vertices[i].x *= viewport_size.y/viewport_size.x; //aspect ratio
    }


    for (int i = 0; i < n-1; ++i) {
        gl_Position = vec4(vertices[i], 1);
        position = vertices[i];
        normal = normals[i];
        EmitVertex();

        gl_Position = vec4(vertices[i + 1], 1);
        normal = normals[i + 1];
        EmitVertex();

        gl_Position = vec4(vertices[i + n], 1);
        normal = normals[i + n];
        EmitVertex();

        gl_Position =  vec4(vertices[i + n + 1], 1);
        normal =  normals[i + n + 1];
        EmitVertex();
        EndPrimitive();
    }

}

void main() {

    build_tube();

}
