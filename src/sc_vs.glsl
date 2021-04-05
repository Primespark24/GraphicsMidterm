#version 450 core  

in vec4 cube_vertex; //Currently being drawn point (of a triangle)

uniform mat4 perspective; // Perspective transform
uniform mat4 toCamera;    // world to Camera transform (should have no translation for skycube)

out vec4 texture_coordinates; //Ouput to fragment shader
                                                                  
void main(void) {
    texture_coordinates = cube_vertex * -1;  //Starting by passing texture coordinates
    //                                  ^^ Flip texture maping around

    //All modifications are pulled in via attributes    
    gl_Position =  perspective * toCamera * cube_vertex;
                            
}                                                                 