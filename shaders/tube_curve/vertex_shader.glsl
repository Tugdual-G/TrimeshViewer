#version 460 core

layout(location = 0) in vec3 in_pos;        // Vertex position
layout(location = 1) in vec3 in_tangent;        // Vertex colors

void main()
{

    gl_Position = vec4(in_pos, 1.0);

}
