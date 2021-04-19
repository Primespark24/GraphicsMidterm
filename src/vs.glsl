#version 450 core

out vec4 vs_color; //Ouput to fragment shader
out vec2 vs_uv;

uniform mat4 transform; //Transformation matrix
uniform mat4 perspective; //Perspective transform
uniform mat4 toCamera; //world to Camera transform

in vec4 obj_vertex; //Currently being drawn point (of a triangle)
in vec2 obj_uv;     //Currently being drawn texture maping of point

void main(void) {
    //All modifications are pulled in via attributes
    gl_Position = perspective * toCamera * transform * obj_vertex;

    vs_uv = obj_uv;
    vs_color = obj_vertex * 2.0 + vec4(0.5,0.5,0.5,0.0);
}
