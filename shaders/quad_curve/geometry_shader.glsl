#version 460 core

layout (lines_adjacency) in;
layout (triangle_strip, max_vertices = 4) out;
in vec3 v_color[];
out vec3 normal;
out vec3 position;
out vec3 color;

uniform float r;
uniform float zoom_level;
uniform vec2 viewport_size;



vec3 normal_vec(vec3 T){
    // Normal vector to the tangent T.
    vec3 normal = vec3(0,0,0);
    normal.x = -T.y;
    normal.y = T.x;
    normal = normalize(normal);
    return normal;
}



void build_quad_line(){

    vec3 pos0 = gl_in[0].gl_Position.xyz;
    vec3 pos1 = gl_in[1].gl_Position.xyz;
    vec3 pos2 = gl_in[2].gl_Position.xyz;
    vec3 pos3 = gl_in[3].gl_Position.xyz;

    // line tangent
    vec3 T1 = normalize(pos2 - pos1 );

    // adjacent tangents
    vec3 T0 = normalize(pos1 - pos0);
    vec3 T2 = normalize(pos3 - pos2);

    vec3 A1 = normal_vec(T0+T1);
    vec3 A2 = normal_vec(T1+T2);


    vec3 N = normal_vec(T1);

    vec3 normals[4];
    vec3 vertices[4];

    float l = r/dot(A1, N);
    l = abs(l) > 2 * r ? 0.0 : l;
    vertices[0] = pos1 - l*A1;
    normals[0] = normalize(-N + vec3(0,0,0.1));
    vertices[1] = pos1 + l*A1;
    normals[1] = normalize(N + vec3(0,0,0.1));

    l = r/dot(A2, N);
    l = abs(l) > 2 * r ? 0.0 : l;
    vertices[2] = pos2 - l*A2;
    normals[2] = normalize(-N + vec3(0,0,0.1));
    vertices[3] = pos2 + l*A2;
    normals[3] = normalize(N + vec3(0,0,0.1));

    for (int i = 0 ; i < 4 ; ++i){
        position = vertices[i];
        vertices[i].xy *= -2/(vertices[i].z - 2); // perspective
        vertices[i].xy = vertices[i].xy * zoom_level;
        vertices[i].x *= viewport_size.y/viewport_size.x; //aspect ratio
        gl_Position = vec4(vertices[i], 1);
        normal = normals[i];
        color = v_color[1+i/3];
        EmitVertex();
    }
    EndPrimitive();

}

void main() {

    build_quad_line();

}
